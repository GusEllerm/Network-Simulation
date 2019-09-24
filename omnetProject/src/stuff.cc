//
//
//    // State machine requires something else to place an appmessage into the buffer
//    // It will wait till something is in the buffer before commencing
//    FSM_Switch(MAC_FSM){
//        case FSM_Exit(INIT):
//            backoffCounter = 0;
//
//            // This is here to sync with the init cycle of the transceiver
//            if (firstPass) {
//                transmissionRequest *initPacket = new transmissionRequest;
//                send(initPacket, "out0");
//                firstPass = false;
//            }
//
//            // if there is something to read from the buffer start the state machine to transmit
//            else if (buffer.size() > 0) {
//                EV << "Somthing in the buffer to send \n";
//                curMessage = new appMessage(*buffer.back());
//                // Start the process to send curMessage
//                FSM_Goto(MAC_FSM, TRANSMITCS);
//            }
//            break;
//
//
//        case FSM_Exit(TRANSMITCS):
//        {
//            // send CSRequest packet to transceiver
//            CSRequest *csrmsg = new CSRequest;
//            send(csrmsg, "out0");
//            FSM_Goto(MAC_FSM, TRANSMITLOCK);
//            break;
//        }
//
//        case FSM_Enter(TRANSMITLOCK):
//            // TODO
//            break;
//        case FSM_Exit(TRANSMITLOCK):
//            // will recieve a CSResponse packet
//            // busyChannel will be either true or false
//            if (dynamic_cast<CSResponse *>(msg)) {
//                EV << "GOT CS RESPONSE IN MAC";
//                CSResponse *csmsg = static_cast<CSResponse *>(msg);
//                if (csmsg->getBusyChannel()) {
//                    // Channel is busy
//                    EV << "Channel is busy \n";
//                    EV << csmsg->getBusyChannel();
//                    EV << "\n";
//
//                    if (backoffCounter <= maxBackoffs) {
//                        backoffCounter += 1;
//                        FSM_Goto(MAC_FSM, TRANSMITCS);
//                    } else {
//                        // packet is dropped
//                        buffer.pop_back();
//                        FSM_Goto(MAC_FSM, INIT);
//                    }
//                } else {
//                    // Channel is clear
//                    EV << "Channel is clear \n";
//                    FSM_Goto(MAC_FSM, TRANSMITMSG);
//                }
//            }
//            break;
//
//        case FSM_Exit(TRANSMITMSG):
//            EV << "TRANSMIT MESSAGE \n";
//            FSM_Goto(MAC_FSM, TRANSMITLOCK); // this is just here to stop the compiler complaning about unfinished code
//            // TODO
//            break;
//
//        case FSM_Exit(TRANSMITFAIL):
//            // TODO
//            break;
//    }
