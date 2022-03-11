import numpy as np
import time

expectedOutputs =   [
                        [1, 1, 0, 1, "False"], 
                        [1, 0, 1, 0, "True"], 
                        [0, 1, 1, 0, "True"], 
                        [0, 0, 0, 1, "False"]
                    ]

lrate           =  1

input_weight11  =  0.2
input_weight12  = -0.3
input_weight21  =  0.4
input_weight22  =  0.3
hiddenNode1     =  0.1
hiddenNode2     = -0.1

hidden_weight11 =  0.3
hidden_weight12 = -0.2
hidden_weight21 =  0.5
hidden_weight22 = -0.4
trueNode        = -0.2
falseNode       =  0.3

def sigmoidFunction(value):
	return 1 / (1 + np.exp(-value))

def feedForwardFunc(weight1, weight2, input1, input2, bias):
    net_input = (weight1*input1)+(weight2*input2)+bias
    return sigmoidFunction(net_input)

def outputErrdrvFunc(value, expected):
    return value * (1-value) * (expected-value)
def hiddenErrdrvFunc(value, trueErrdrv, falseErrdrv, weight1, weight2):
    return value * (1-value) * ((trueErrdrv*weight1)+(falseErrdrv*weight2))

def biasAdjustment(curBias, errdrv):
    return curBias + errdrv
def weightAdjustment(curWeight, nodeValue, outputErrdrv):
    return curWeight + (lrate * outputErrdrv) * nodeValue

for epoch in range(1,5001):
    print("EPOCH: %d" %(epoch))
    for outputs in expectedOutputs:
    #     hiddenNodeOneValue = sigmoidFunction((input_weight11 * outputs[0]) + (input_weight21 * outputs[0]) + hiddenNode1)
    #     hiddenNodeTwoValue = sigmoidFunction((input_weight12 * outputs[1]) + (input_weight22 * outputs[1]) + hiddenNode2)
    #     trueNodeValue = sigmoidFunction((hidden_weight11 * hiddenNodeOneValue) + (hidden_weight21 * hiddenNodeTwoValue) + trueNode)
    #     falseNodeValue = sigmoidFunction((hidden_weight12 * hiddenNodeOneValue) + (hidden_weight22 * hiddenNodeTwoValue) + falseNode)

    #     print("")
    #     print("node,True  %d,%d: %f" % (outputs[0], outputs[1], trueNodeValue))
    #     print("node,False %d,%d: %f" % (outputs[0], outputs[1], falseNodeValue))

        #find node values
        hiddenNodeOneValue = feedForwardFunc(input_weight11, input_weight21, outputs[0], outputs[1], hiddenNode1)
        hiddenNodeTwoValue = feedForwardFunc(input_weight12, input_weight22, outputs[0], outputs[1], hiddenNode2)
        trueNodeValue = feedForwardFunc(hidden_weight11, hidden_weight21, hiddenNodeOneValue, hiddenNodeTwoValue, trueNode)
        falseNodeValue = feedForwardFunc(hidden_weight12, hidden_weight22, hiddenNodeOneValue, hiddenNodeTwoValue, falseNode)

        #find error derivations
        trueNodeErrdrv = outputErrdrvFunc(trueNodeValue, outputs[2])
        falseNodeErrdrv = outputErrdrvFunc(falseNodeValue, outputs[3])
        hiddenNodeOneErrdrv = hiddenErrdrvFunc(hiddenNodeOneValue,  trueNodeErrdrv, falseNodeErrdrv, hidden_weight11, hidden_weight12)
        hiddenNodeTwoErrdrv = hiddenErrdrvFunc(hiddenNodeTwoValue, trueNodeErrdrv, falseNodeErrdrv, hidden_weight21, hidden_weight22)

        #set new node values
        trueNode = biasAdjustment(trueNode, trueNodeErrdrv)
        falseNode = biasAdjustment(falseNode, falseNodeErrdrv)
        hiddenNode1 = biasAdjustment(hiddenNode1, hiddenNodeOneErrdrv)
        hiddenNode2 = biasAdjustment(hiddenNode2, hiddenNodeTwoErrdrv)

        #set new weight values 
        hidden_weight11 = weightAdjustment(hidden_weight11, hiddenNodeOneValue, trueNodeErrdrv)
        hidden_weight12 = weightAdjustment(hidden_weight12, hiddenNodeOneValue, falseNodeErrdrv)
        hidden_weight21 = weightAdjustment(hidden_weight21, hiddenNodeTwoValue, trueNodeErrdrv)
        hidden_weight22 = weightAdjustment(hidden_weight22, hiddenNodeTwoValue, falseNodeErrdrv)
        #weightAdjustment(curWeight, nodeValue, outputErrdrv)
        input_weight11 = weightAdjustment(input_weight11, outputs[0], hiddenNodeOneErrdrv)
        input_weight12 = weightAdjustment(input_weight12, outputs[0], hiddenNodeTwoErrdrv)
        input_weight21 = weightAdjustment(input_weight21, outputs[1], hiddenNodeOneErrdrv)
        input_weight22 = weightAdjustment(input_weight22, outputs[1], hiddenNodeTwoErrdrv)

        print("Input [%d, %d] %s" % (outputs[0], outputs[1], outputs[4]))
        print("Expected\t\t True %d output %f" % (outputs[2], trueNodeValue))
        print("Expected\t\tFalse %d output %f\n" % (outputs[3], falseNodeValue))
    # time.sleep(.25)
    
# #do the error printing
# print("OUTPUT BEGINS")
# #(trueNode, newTrueNodeValue, falseNode, newFalseNodeValue)
# #SHOULD BE
# #(-0.2, -0.348, -0.3, 0.43)
# print("Output Nodes: True(%f -> %f) False(%f -> %f)" % (trueNode, newTrueNodeValue, falseNode, newFalseNodeValue))
# #(hidden_weight11, new_h2o_weight_11, hidden_weight12, new_h2o_weight_12)
# #SHOULD BE
# #(0.3, 0.21, -0.2, -0.11)
# print("HiddenToOutput Weights: Hidden11(%f -> %f) Hidden12(%f -> %f)" % (hidden_weight11, new_h2o_weight_11, hidden_weight12, new_h2o_weight_12))
# #(hidden_weight21, new_h2o_weight_21, hidden_weight22, new_h2o_weight_22)
# #SHOULD BE
# #(0.5, 0.43, -0.4, -0.34)
# print("HiddenToOutput Weights: Hidden21(%f -> %f) Hidden22(%f -> %f)" % (hidden_weight21, new_h2o_weight_21, hidden_weight22, new_h2o_weight_22))
# #(hiddenNode1, newHiddenNodeOneValue, hiddenNode2, newHiddenNodeTwoValue)
# #SHOULD BE
# #(0.1, 0.085, -0.1, -0.131)
# print("Hidden Nodes: HiddenOne(%f -> %f) HiddenTwo(%f -> %f)" % (hiddenNode1, newHiddenNodeOneValue, hiddenNode2, newHiddenNodeTwoValue))
# #(input_weight11, new_i2h_weight_11, input_weight12, new_i2h_weight_12)
# #SHOULD BE
# #(0.2, 0.18, -0.3, -0.315)
# print("InputToHidden Weights: Input11(%f -> %f) Input12(%f -> %f)" % (input_weight11, new_i2h_weight_11, input_weight12, new_i2h_weight_12))
# #(input_weight21, new_i2h_weight_21, input_weight22, new_i2h_weight_22)
# #SHOULD BE
# #(0.4, 0.369, 0.3, 0.269)
# print("InputToHidden Weights: Input21(%f -> %f) Input22(%f -> %f)" % (input_weight21, new_i2h_weight_21, input_weight22, new_i2h_weight_22))






