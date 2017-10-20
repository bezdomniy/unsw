import sys
import gym
import tensorflow as tf
import numpy as np
import random
import datetime
import os
from collections import deque
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'

"""
Hyper Parameters
"""
GAMMA = 0.99  # discount factor for target Q
INITIAL_EPSILON = 0.6  # starting value of epsilon
FINAL_EPSILON = 0.01  # final value of epsilon
EPSILON_DECAY_STEPS = 100
REPLAY_SIZE = 100000  # experience replay buffer size
BATCH_SIZE = 128  # size of minibatch
TEST_FREQUENCY = 500  # How many episodes to run before visualizing test accuracy
SAVE_FREQUENCY = 1000  # How many episodes to run before saving model (unused)
NUM_EPISODES = 800  # Episode limitation
EP_MAX_STEPS = 1000  # Step limitation in an episode
# The number of test iters (with epsilon set to 0) to run every TEST_FREQUENCY episodes
NUM_TEST_EPS = 100
HIDDEN_NODES = 64
TARGET_UPDATE_FREQ = 20

AVERAGE_OVER = 100
latest_100 = deque(maxlen=AVERAGE_OVER)

def init(env, env_name):
    """
    Initialise any globals, e.g. the replay_buffer, epsilon, etc.
    return:
        state_dim: The length of the state vector for the env
        action_dim: The length of the action space, i.e. the number of actions

    NB: for discrete action envs such as the cartpole and mountain car, this
    function can be left unchanged.

    Hints for envs with continuous action spaces, e.g. "Pendulum-v0"
    1) you'll need to modify this function to discretise the action space and
    create a global dictionary mapping from action index to action (which you
    can use in `get_env_action()`)
    2) for Pendulum-v0 `env.action_space.low[0]` and `env.action_space.high[0]`
    are the limits of the action space.
    3) setting a global flag iscontinuous which you can use in `get_env_action()`
    might help in using the same code for discrete and (discretised) continuous
    action spaces
    """
    global replay_buffer, epsilon, iscontinuous, action_map
    replay_buffer = []
    epsilon = INITIAL_EPSILON

    iscontinuous = not isinstance(env.action_space,gym.spaces.discrete.Discrete)

    state_dim = env.observation_space.shape[0]

    if iscontinuous:
        action_map = dict()
        BIN_FACTOR = 100
        action_dim = int((env.action_space.high[0] - env.action_space.low[0]) * BIN_FACTOR)
        values = np.arange(0,action_dim,1/BIN_FACTOR)
        for i in range(action_dim):
            action_map[i] = values[i]

    else:
        action_dim = env.action_space.n
    return state_dim, action_dim


def get_target_network(state_dim, action_dim,train_weights, hidden_nodes=HIDDEN_NODES):
    state_in = tf.placeholder("float", [None, state_dim])
    action_in = tf.placeholder("float", [None, action_dim])  # one hot
    target_in = tf.placeholder("float",
                               [None])  # q value for the target network

    initializer = tf.random_normal_initializer(0., 0.1)
    
    with tf.variable_scope("target_network"):
        layer1 = tf.layers.dense(state_in,hidden_nodes,activation=tf.nn.relu,
                                kernel_initializer=initializer)
        layer2 = tf.layers.dense(layer1,hidden_nodes,activation=tf.nn.relu,
                                kernel_initializer=initializer)
        q_values = tf.layers.dense(layer2,action_dim,activation=None,
                                    kernel_initializer=initializer)

    q_selected_action = \
        tf.reduce_sum(tf.multiply(q_values, action_in), reduction_indices=1)

    loss = tf.reduce_mean(tf.square(target_in - q_selected_action,name="loss"))

    update_weights_op = copy_vars_op(train_weights)

    return q_values, update_weights_op                           

def copy_vars_op(train_weights):
    targets = tf.get_collection(tf.GraphKeys.GLOBAL_VARIABLES, scope='target_network')
    assign_ops = []
    for i in range(len(targets)):
        v = train_weights[i].eval()
        assign_ops.append(targets[i].assign(v))
    return tf.group(assign_ops)


def get_network(state_dim, action_dim, hidden_nodes=HIDDEN_NODES):
    """Define the neural network used to approximate the q-function

    The suggested structure is to have each output node represent a Q value for
    one action. e.g. for cartpole there will be two output nodes.

    Hints:
    1) Given how q-values are used within RL, is it necessary to have output
    activation functions?
    2) You will set `target_in` in `get_train_batch` further down. Probably best
    to implement that before implementing the loss (there are further hints there)
    """
    state_in = tf.placeholder("float", [None, state_dim])
    action_in = tf.placeholder("float", [None, action_dim])  # one hot
    target_in = tf.placeholder("float",
                               [None])  # q value for the target network

    # TO IMPLEMENT: Q network, whose input is state_in, and has action_dim outputs
    # which are the network's esitmation of the Q values for those actions and the
    # input state. The final layer should be assigned to the variable q_values
    initializer = tf.random_normal_initializer(0., 0.1)
    
    with tf.variable_scope("q_network"):
        layer1 = tf.layers.dense(state_in,hidden_nodes,activation=tf.nn.relu,
                                kernel_initializer=initializer)
        layer2 = tf.layers.dense(layer1,hidden_nodes,activation=tf.nn.relu,
                                kernel_initializer=initializer)
        q_values = tf.layers.dense(layer2,action_dim,activation=None,
                                    kernel_initializer=initializer)

    q_selected_action = \
        tf.reduce_sum(tf.multiply(q_values, action_in), reduction_indices=1)

    # TO IMPLEMENT: loss function
    # should only be one line, if target_in is implemented correctly

    loss = tf.reduce_mean(tf.square(target_in - q_selected_action,name="loss"))

    optimise_step = tf.train.AdamOptimizer().minimize(loss)

    train_loss_summary_op = tf.summary.scalar("TrainingLoss", loss)
    return state_in, action_in, target_in, q_values, q_selected_action, \
           loss, optimise_step, train_loss_summary_op


def init_session():
    global session, writer
    session = tf.InteractiveSession()
    session.run(tf.global_variables_initializer())

    # Setup Logging
    logdir = "tensorboard/" + datetime.datetime.now().strftime(
        "%Y%m%d-%H%M%S") + "/"
    writer = tf.summary.FileWriter(logdir, session.graph)


def get_action(state, state_in, q_values, epsilon, test_mode, action_dim):
    Q_estimates = q_values.eval(feed_dict={state_in: [state]})[0]
    epsilon_to_use = 0.0 if test_mode else epsilon
    if random.random() < epsilon_to_use:
        action = random.randint(0, action_dim - 1)
    else:
        action = np.argmax(Q_estimates)
    return action


def get_env_action(action):
    """
    Modify for continous action spaces that you have discretised, see hints in
    `init()`
    """
    if iscontinuous:
        action=[action_map[action]]

    return action


def update_replay_buffer(replay_buffer, state, action, reward, next_state, done,
                         action_dim):
    """
    Update the replay buffer with provided input in the form:
    (state, one_hot_action, reward, next_state, done)

    Hint: the minibatch passed to do_train_step is one entry (randomly sampled)
    from the replay_buffer
    """
    # TO IMPLEMENT: append to the replay_buffer
    # ensure the action is encoded one hot
    one_hot_action = np.int32(np.eye(action_dim)[action])
    # append to buffer
    replay_buffer.append([state, one_hot_action, reward, next_state, done])
    # Ensure replay_buffer doesn't grow larger than REPLAY_SIZE
    if len(replay_buffer) > REPLAY_SIZE:
        replay_buffer.pop(0)
    return None


def do_train_step(replay_buffer, state_in, action_in, target_in,
                  q_values, q_selected_action, loss, optimise_step,
                  train_loss_summary_op, batch_presentations_count):
    minibatch = random.sample(replay_buffer, BATCH_SIZE)
    target_batch, state_batch, action_batch = \
        get_train_batch(q_values, state_in, minibatch)

    summary, _ = session.run([train_loss_summary_op, optimise_step], feed_dict={
        target_in: target_batch,
        state_in: state_batch,
        action_in: action_batch
    })
    writer.add_summary(summary, batch_presentations_count)


def get_train_batch(q_values, state_in, minibatch):
    """
    Generate Batch samples for training by sampling the replay buffer"
    Batches values are suggested to be the following;
        state_batch: Batch of state values
        action_batch: Batch of action values
        target_batch: Target batch for (s,a) pair i.e. one application
            of the bellman update rule.

    return:
        target_batch, state_batch, action_batch

    Hints:
    1) To calculate the target batch values, you will need to use the
    q_values for the next_state for each entry in the batch.
    2) The target value, combined with your loss defined in `get_network()` should
    reflect the equation in the middle of slide 12 of Deep RL 1 Lecture
    notes here: https://webcms3.cse.unsw.edu.au/COMP9444/17s2/resources/12494
    """
    state_batch = [data[0] for data in minibatch]
    action_batch = [data[1] for data in minibatch]
    reward_batch = [data[2] for data in minibatch]
    next_state_batch = [data[3] for data in minibatch]

    target_batch = []
    Q_value_batch = q_values.eval(feed_dict={
        state_in: next_state_batch
    })
    
    for i in range(0, BATCH_SIZE):
        sample_is_done = minibatch[i][4]
        if sample_is_done:
            target_batch.append(reward_batch[i])
        else:
            # TO IMPLEMENT: set the target_val to the correct Q value update
            maxQ = np.max(Q_value_batch[i])
            target_val = reward_batch[i]+ GAMMA*maxQ
            target_batch.append(target_val)
    return target_batch, state_batch, action_batch


def qtrain(env, state_dim, action_dim,
           state_in, action_in, target_in, q_values, q_selected_action,
           loss, optimise_step, train_loss_summary_op, eval_q_values, update_weights,
           num_episodes=NUM_EPISODES, ep_max_steps=EP_MAX_STEPS,
           test_frequency=TEST_FREQUENCY, num_test_eps=NUM_TEST_EPS,
           final_epsilon=FINAL_EPSILON, epsilon_decay_steps=EPSILON_DECAY_STEPS,
           force_test_mode=False, render=True):
    global epsilon
    # Record the number of times we do a training batch, take a step, and
    # the total_reward across all eps
    batch_presentations_count = total_steps = total_reward = 0

    for episode in range(num_episodes):
        # initialize task
        state = env.reset()
        if render: env.render()

        # Update epsilon once per episode - exp decaying
        epsilon -= (epsilon - final_epsilon) / epsilon_decay_steps

        # in test mode we set epsilon to 0
        test_mode = force_test_mode or \
                    ((episode % test_frequency) < num_test_eps and
                        episode > num_test_eps
                    )
        if test_mode: print("Test mode (epsilon set to 0.0)")

        ep_reward = 0
        for step in range(ep_max_steps):
            total_steps += 1

            # get an action and take a step in the environment
            action = get_action(state, state_in, q_values, epsilon, test_mode,
                                action_dim)
            env_action = get_env_action(action)
            next_state, reward, done, _ = env.step(env_action)
            ep_reward += reward

            # display the updated environment
            if render: env.render()  # comment this line to possibly reduce training time

            # add the s,a,r,s' samples to the replay_buffer
            update_replay_buffer(replay_buffer, state, action, reward,
                                 next_state, done, action_dim)
            state = next_state

            # perform a training step if the replay_buffer has a batch worth of samples
            if (len(replay_buffer) > BATCH_SIZE):
                if episode % TARGET_UPDATE_FREQ == 0:
                    session.run(update_weights)

                do_train_step(replay_buffer, state_in, action_in, target_in,
                              eval_q_values, q_selected_action, loss, optimise_step,
                              train_loss_summary_op, batch_presentations_count)
                #print(q_selected_action)
                batch_presentations_count += 1

            if done:
                break
        total_reward += ep_reward

        if len(latest_100) > AVERAGE_OVER:
            latest_100.popleft()
        latest_100.append(ep_reward)
        avg_reward = np.mean(latest_100)

        test_or_train = "test" if test_mode else "train"
        print("end {0} episode {1}, ep reward: {2}, ave reward: {3}, \
            Batch presentations: {4}, epsilon: {5}".format(
            #test_or_train, episode, ep_reward, total_reward / (episode + 1),
            test_or_train, episode, ep_reward, avg_reward,
            batch_presentations_count, epsilon
        ))

        if avg_reward >= 195:
            print("COMPLETE")
            break


def setup():
    #default_env_name = 'CartPole-v0'
    #default_env_name = 'MountainCar-v0'
    default_env_name = 'Pendulum-v0'
    # if env_name provided as cmd line arg, then use that
    env_name = sys.argv[1] if len(sys.argv) > 1 else default_env_name
    env = gym.make(env_name)
    state_dim, action_dim = init(env, env_name)
    network_vars = get_network(state_dim, action_dim)
    train_weights = tf.get_collection(tf.GraphKeys.GLOBAL_VARIABLES, scope='q_network')
    eval_q_values, update_weights = get_target_network(state_dim, action_dim,train_weights)
    #print([v.name for v in tf.get_collection(tf.GraphKeys.GLOBAL_VARIABLES, scope='target_network')])
    #print(train_weight_names)
    init_session()
    return env, state_dim, action_dim, network_vars, eval_q_values, update_weights


def main():
    env, state_dim, action_dim, network_vars, eval_q_values, update_weights = setup()
    qtrain(env, state_dim, action_dim, *network_vars,eval_q_values, update_weights, render=False)


if __name__ == "__main__":
    main()
