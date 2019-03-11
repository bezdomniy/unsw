using System;
using System.Threading.Tasks;
using System.Collections.Generic;
using System.Numerics;
using Nethereum.Hex.HexTypes;
using Nethereum.ABI.FunctionEncoding.Attributes;
using Nethereum.Web3;
using Nethereum.RPC.Eth.DTOs;
using Nethereum.Contracts.CQS;
using Nethereum.Contracts.ContractHandlers;
using Nethereum.Contracts;
using System.Threading;
using Message.Contracts.Message.ContractDefinition;

namespace Message.Contracts.Message
{
    public partial class MessageService
    {
        public static Task<TransactionReceipt> DeployContractAndWaitForReceiptAsync(Nethereum.Web3.Web3 web3, MessageDeployment messageDeployment, CancellationTokenSource cancellationTokenSource = null)
        {
            return web3.Eth.GetContractDeploymentHandler<MessageDeployment>().SendRequestAndWaitForReceiptAsync(messageDeployment, cancellationTokenSource);
        }
        public static Task<string> DeployContractAsync(Nethereum.Web3.Web3 web3, MessageDeployment messageDeployment)
        {
            return web3.Eth.GetContractDeploymentHandler<MessageDeployment>().SendRequestAsync(messageDeployment);
        }
        public static async Task<MessageService> DeployContractAndGetServiceAsync(Nethereum.Web3.Web3 web3, MessageDeployment messageDeployment, CancellationTokenSource cancellationTokenSource = null)
        {
            var receipt = await DeployContractAndWaitForReceiptAsync(web3, messageDeployment, cancellationTokenSource);
            return new MessageService(web3, receipt.ContractAddress);
        }

        protected Nethereum.Web3.Web3 Web3{ get; }

        public ContractHandler ContractHandler { get; }

        public MessageService(Nethereum.Web3.Web3 web3, string contractAddress)
        {
            Web3 = web3;
            ContractHandler = web3.Eth.GetContractHandler(contractAddress);
        }

        public Task<string> SetMessageRequestAsync(SetMessageFunction setMessageFunction)
        {
             return ContractHandler.SendRequestAsync(setMessageFunction);
        }

        public Task<TransactionReceipt> SetMessageRequestAndWaitForReceiptAsync(SetMessageFunction setMessageFunction, CancellationTokenSource cancellationToken = null)
        {
             return ContractHandler.SendRequestAndWaitForReceiptAsync(setMessageFunction, cancellationToken);
        }

        public Task<string> SetMessageRequestAsync(string newMessage)
        {
            var setMessageFunction = new SetMessageFunction();
                setMessageFunction.NewMessage = newMessage;
            
             return ContractHandler.SendRequestAsync(setMessageFunction);
        }

        public Task<TransactionReceipt> SetMessageRequestAndWaitForReceiptAsync(string newMessage, CancellationTokenSource cancellationToken = null)
        {
            var setMessageFunction = new SetMessageFunction();
                setMessageFunction.NewMessage = newMessage;
            
             return ContractHandler.SendRequestAndWaitForReceiptAsync(setMessageFunction, cancellationToken);
        }

        public Task<string> MessageQueryAsync(MessageFunction messageFunction, BlockParameter blockParameter = null)
        {
            return ContractHandler.QueryAsync<MessageFunction, string>(messageFunction, blockParameter);
        }

        
        public Task<string> MessageQueryAsync(BlockParameter blockParameter = null)
        {
            return ContractHandler.QueryAsync<MessageFunction, string>(null, blockParameter);
        }
    }
}
