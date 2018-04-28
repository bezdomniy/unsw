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
INITIAL_EPSILON = 0.8  # starting value of epsilon
FINAL_EPSILON = 0.01  # final value of epsilon
EPSILON_DECAY_STEPS = 200
REPLAY_SIZE = 100000  # experience replay buffer size
BATCH_SIZE = 128  # size of minibatch
TEST_FREQUENCY = 1000  # How many episodes to run before visualizing test accuracy
SAVE_FREQUENCY = 1000  # How many episodes to run before saving model (unused)
# The number of test iters (with epsilon set to 0) to run every TEST_FREQUENCY episodes
NUM_TEST_EPS = 100
NUM_EPISODES = TEST_FREQUENCY + NUM_TEST_EPS  # Episode limitation
EP_MAX_STEPS = 1000  # Step limitation in an episode

HIDDEN_NODES = 256

FIRST_TARGET_UPDATE = 0
TARGET_UPDATE_FREQ = 1000 # How often to update target network weights

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
    global replay_buffer, epsilon, iscontinuous, action_map, action_dim
    replay_buffer = []
    epsilon = INITIAL_EPSILON

    state_dim = env.observation_space.shape[0]

    iscontinuous = not isinstance(env.action_space,gym.spaces.discrete.Discrete)

    if iscontinuous:
        action_map = dict()
        BIN_FACTOR = 100
        action_dim = int((env.action_space.high[0] - env.action_space.low[0]) * BIN_FACTOR +1)
        values = np.arange(env.action_space.low[0],env.action_space.high[0]+1,1/BIN_FACTOR)
        for i in range(action_dim):
            action_map[i] = values[i]

    else:
        action_dim = env.action_space.n

    return state_dim, action_dim


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

    train_loss_summary_op = tf.summary.scalar("TrainingLoss", tf.reduce_mean(loss))
    return state_in, action_in, target_in, q_values, q_selected_action, \
           loss, optimise_step, train_loss_summary_op


def get_target_network(state_dim, action_dim, hidden_nodes=HIDDEN_NODES):
    state_in = tf.placeholder("float", [None, state_dim])
    action_in = tf.placeholder("float", [None, action_dim])

    w1 = tf.placeholder("float", [state_dim, hidden_nodes])
    b1 = tf.placeholder("float", [hidden_nodes])
    w2 = tf.placeholder("float", [hidden_nodes, hidden_nodes])
    b2 = tf.placeholder("float", [hidden_nodes])
    w3 = tf.placeholder("float", [hidden_nodes, action_dim])
    b3 = tf.placeholder("float", [action_dim])

    train_weights = w1, b1, w2, b2, w3, b3
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
    update_weights_op = copy_vars_op(train_weights)

    return state_in, action_in, q_selected_action, update_weights_op, train_weights                    

def copy_vars_op(train_weights):
    targets = tf.get_collection(tf.GraphKeys.GLOBAL_VARIABLES, scope='target_network')
    assign_ops = []
    for i in range(len(targets)):
        v = train_weights[i]
        op = targets[i].assign(v)
        assign_ops.append(op)

    return assign_ops
    #return tf.group(*assign_ops)

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
    global replay_priority_total
    # TO IMPLEMENT: append to the replay_buffer
    # ensure the action is encoded one hot
    one_hot_action = np.int32(np.eye(action_dim)[action])

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
        get_train_batch(q_values,state_in, minibatch)

    summary, _, new_loss = session.run([train_loss_summary_op,  optimise_step,loss], feed_dict={
        target_in: target_batch,
        state_in: state_batch,
        action_in: action_batch
    })

    writer.add_summary(summary, batch_presentations_count)


def get_train_batch(q_values,state_in, minibatch):
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
    Q_values_batch = q_values.eval(feed_dict={
        state_in: next_state_batch
    })

    Q_action_batch = np.argmax(Q_values_batch,1)
    Q_action_batch = [np.eye(action_dim)[x] for x in Q_action_batch]

    Q_eval = q_eval_action.eval(feed_dict={
        state_in_eval: next_state_batch,
        action_in_eval: Q_action_batch
    })
    
    
    for i in range(0, BATCH_SIZE):
        sample_is_done = minibatch[i][4]
        if sample_is_done:
            target_batch.append(reward_batch[i])
        else:
            # TO IMPLEMENT: set the target_val to the correct Q value update
            target_val = reward_batch[i]+ GAMMA*Q_eval[i]
            target_batch.append(target_val)
    
    return target_batch, state_batch, action_batch


def qtrain(env, state_dim, action_dim,
           state_in, action_in, target_in, q_values, q_selected_action,
           loss, optimise_step, train_loss_summary_op,
           num_episodes=NUM_EPISODES, ep_max_steps=EP_MAX_STEPS,
           test_frequency=TEST_FREQUENCY, num_test_eps=NUM_TEST_EPS,
           final_epsilon=FINAL_EPSILON, epsilon_decay_steps=EPSILON_DECAY_STEPS,
           force_test_mode=False, render=True):
    global batch_init, epsilon,Q_loss, q_eval_action,update_weights_op, train_weights, state_in_eval, action_in_eval    # Record the number of times we do a training batch, take a step, and
    # the total_reward across all eps
    batch_presentations_count = total_steps = total_reward = 0

    state_in_eval, action_in_eval, q_eval_action, update_weights_op, train_weights = get_target_network(state_dim, action_dim)
    q_network_vars = tf.get_collection(tf.GraphKeys.GLOBAL_VARIABLES, scope='q_network')[:6]

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

        #if (episode == 0 or episode > FIRST_TARGET_UPDATE) and (episode % TARGET_UPDATE_FREQ == 0):
        if (episode == 0):
                weight_upates = [x.eval() for x in q_network_vars]
                for i in range(len(update_weights_op)):
                    result= session.run(update_weights_op[i],
                        feed_dict={train_weights[i]:weight_upates[i]})

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

            update_replay_buffer(replay_buffer, state, action, reward,
                                 next_state, done, action_dim)
            state = next_state

            # perform a training step if the replay_buffer has a batch worth of samples
            if (len(replay_buffer) > BATCH_SIZE and not test_mode):
                do_train_step(replay_buffer, state_in, action_in, target_in,
                              q_values, q_selected_action, loss, optimise_step,
                              train_loss_summary_op, batch_presentations_count)
                batch_presentations_count += 1


            if (total_steps > FIRST_TARGET_UPDATE) and (total_steps % TARGET_UPDATE_FREQ == 0) and not test_mode:
                #print("============== COPYING WEIGHTS ===============")
                weight_upates = [x.eval() for x in q_network_vars]
                for i in range(len(update_weights_op)):
                    result= session.run(update_weights_op[i],
                        feed_dict={train_weights[i]:weight_upates[i]})

            if done:
                break
        total_reward += ep_reward

        if len(latest_100) > AVERAGE_OVER:
            latest_100.popleft()
        latest_100.append(ep_reward)
        avg_reward = np.mean(latest_100)

        test_or_train = "test" if test_mode else "train"
        #print(replay_buffer.qsize())
        print("end {0} episode {1}, ep reward: {2}, ave reward: {3}, \
            Batch presentations: {4}, epsilon: {5}, total_steps: {6}".format(
            #test_or_train, episode, ep_reward, total_reward / (episode + 1),
            test_or_train, episode, ep_reward, avg_reward,
            batch_presentations_count, epsilon, total_steps
        ))

        #if avg_reward >= 195:
        #    print("COMPLETE")
        #    break


def setup():
    default_env_name = 'CartPole-v0'
    #default_env_name = 'MountainCar-v0'
    #default_env_name = 'Pendulum-v0'
    # if env_name provided as cmd line arg, then use that
    env_name = sys.argv[1] if len(sys.argv) > 1 else default_env_name
    env = gym.make(env_name)
    state_dim, action_dim = init(env, env_name)
    network_vars = get_network(state_dim, action_dim)
    init_session()
    return env, state_dim, action_dim, network_vars


def main():
    env, state_dim, action_dim, network_vars = setup()
    qtrain(env, state_dim, action_dim, *network_vars, render=False)


if __name__ == "__main__":
    main()