#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>

using namespace std;






struct transition{
    string startState;
    string readSymbol;
    string replaceSymbol;
    string direction;
    string endState;

};

// check these conditions in functions,
// to make the code more readable
// and easier to maintain
template<typename ELEMENT,typename CONTAINER>
auto isItAcceptState(ELEMENT& currentState, ELEMENT& acceptState, CONTAINER& statesList){
    if (currentState == acceptState){
        cout<<"ROUT: ";
        for(auto& element: statesList){
            cout<<element<<" ";
        }
        cout<<endl;
        cout<<"RESULT: accepted"<<endl;
        return true;
    } else{
        return false;
    }
}
template<typename ELEMENT, typename CONTAINER>
auto isItRejectState(ELEMENT& currentState, ELEMENT& rejectState, CONTAINER& statesList){
    if (currentState == rejectState){
        cout<<"ROUT: ";
        for(auto& element: statesList){
            cout<<element<<" ";
        }
        cout<<endl;
        cout<<"RESULT: rejected"<<endl;
        return true;
    } else{
        return false;
    }
}

template<typename Container, typename ELEMENT>
auto inContainer(Container& container, ELEMENT& state, ELEMENT& readSymbol){
    auto seenTimes = 0;
    for (auto& element: container) {
        if (element.startState == state && element.readSymbol == readSymbol){
            seenTimes ++;
        }
    }
    return seenTimes;
}


template<typename CONTAINER, typename ELEMENT>
auto returnTransition(CONTAINER& container, ELEMENT& state, ELEMENT& readSymbol){
    auto toReturn = transition{};
    for (auto& entry: container) {
        if(entry.startState == state && entry.readSymbol == readSymbol){
            toReturn = entry;
        }
    }
    return toReturn;
}


struct TuringMachine{
    std::vector<std::string> TMinputAlphabet;
    std::vector<std::string> TMtapeAlphabet;
    std::string TMblankSymbol;
    std::vector<std::string> TMstates;
    std::string TMstartState;
    std::string TMacceptState;
    std::string TMrejectState;
    std::vector<transition> TMtransitions;
    std::vector<std::string> TMinputString;
    std::vector<std::string> tape;
    std::vector<std::string> statesPassed;
    int loopCheckNumber;
    int tapeIndex = 0;

    TuringMachine(std::vector<std::string> InputAlphabetVector,
    std::vector<std::string> TapeAlphabetVector,
    std::vector<std::string> BlankSymbolVector,
    std::vector<std::string> StatesVector,
    std::vector<std::string> StartStateVector,
    std::vector<std::string> AcceptStateVector,
    std::vector<std::string> RejectStateVector,
    std::vector<std::string> TransitionsVector,
    std::vector<std::string> InputStringVector):TMinputAlphabet{InputAlphabetVector},
    TMtapeAlphabet{TapeAlphabetVector},TMstates{StatesVector}{
        for(auto& entry: InputStringVector){
            for(auto& entry2: entry){
                TMinputString.push_back(std::string(1,entry2));
            }
        }
        for(auto& entry: BlankSymbolVector){
            TMblankSymbol.append(entry);
        }
        for(auto& entry:StartStateVector){
            TMstartState.append(entry);
        }
        for(auto& entry: AcceptStateVector){
            TMacceptState.append(entry);
        }
        for(auto& entry: RejectStateVector){
            TMrejectState.append(entry);
        }
        for(auto& entry: TMinputString){
            tape.push_back(entry);
        }
        auto sz = TransitionsVector.size();
        for (int i = 0; i < sz; ++i) {
            auto tempTransition =  transition{};
            std::stringstream tempString(TransitionsVector[i]);
            tempString>>tempTransition.startState>>tempTransition.readSymbol>>tempTransition.replaceSymbol>>
            tempTransition.direction>>tempTransition.endState;

            TMtransitions.push_back(tempTransition);
        }
        loopCheckNumber = TMtransitions.size() * TMinputString.size() + 1;
    }
    auto testTheString(){
        // handle the transitions in a while loop...
        bool continueToRun = true;
        auto currentState = TMstartState;
        //cout<<"current state: "<<currentState<<endl;
        statesPassed.push_back(currentState);
        while (continueToRun){
            // check if a transition exists with the current state & read symbol
            auto doesExists = inContainer(TMtransitions, currentState, tape[tapeIndex]);
            if (doesExists >= 1){
                loopCheckNumber --;
                if (loopCheckNumber == 0){
                    cout<<"ROUT: ";
                    for(auto& element: statesPassed){
                        cout<<element<<" ";
                    }
                    cout<<endl;
                    cout<<"RESULT: looped"<<endl;
                    continueToRun = false;
                }
                auto currentTransition = returnTransition(TMtransitions, currentState, tape[tapeIndex]);
                    // replace the tape symbol
                    //print to test
                    tape[tapeIndex] = currentTransition.replaceSymbol;
                    if(currentTransition.direction == "R"){
                        // check weather we reached the end of tape
                        auto sz = tape.size();
                        if (sz == (tapeIndex + 1)){
                            tape.push_back(TMblankSymbol);
                        }
                        tapeIndex ++;
                        currentState = currentTransition.endState;
                        //cout<<"current state: "<<currentState<<endl;
                        //cout<<"new symbol : "<<tape[tapeIndex]<<endl;
                        statesPassed.push_back(currentState);
                    }else{
                        if (tapeIndex == 0){
                            currentState = currentTransition.endState;
                            statesPassed.push_back(currentState);
                            // if we reached accept or reject state exit program
                        }else{
                            tapeIndex --;
                            currentState = currentTransition.endState;
                            statesPassed.push_back(currentState);
                            // if we reached accept or reject state exit program
                        }
                    }
                auto didAccept = isItAcceptState(currentState,TMacceptState, statesPassed);
                auto didReject = isItRejectState(currentState,TMrejectState, statesPassed);
                if (didAccept || didReject){
                    continueToRun = false;
                }
            } else{
                cout<<"Transaction not found!"<<endl;
                cout<<"Please check the structure of the input file"<<endl;
                continueToRun = false;
            }
        }
    }
};


int main()
{
    std::ifstream file("Input_ALPEREN_AKYOL_S018812.txt");
    std::string str;

    // information vectors for the Turing Machine
    std::vector<std::string> InputAlphabetVector;
    std::vector<std::string> TapeAlphabetVector;
    std::vector<std::string> BlankSymbolVector;
    std::vector<std::string> StatesVector;
    std::vector<std::string> StartStateVector;
    std::vector<std::string> AcceptStateVector;
    std::vector<std::string> RejectStateVector;
    std::vector<std::string> TransitionsVector;
    std::vector<std::string> InputStringVector;


    std::vector<std::string> v1;
    while (std::getline(file, str))
    {
        v1.push_back(str);
    }
    std::string buildInfos[10] = {"INPUT-ALPHABET","TAPE-ALPHABET",
                                 "BLANK-SYMBOL","STATES",
                                 "START-STATE","ACCEPT-STATE",
                                 "REJECT-STATE","TRANSITIONS",
                                 "INPUT-STRING","END"};
    for (int i = 0; i < 9; i++)
    {
        int start;
        int end;
        auto sz = v1.size();
        for (int j = 0; j < sz; j++)
        {
            if (v1[j]==buildInfos[i])
            {
                start = j;
                //std::cout<<"found!"<<std::endl;
            }
            else if (v1[j]==buildInfos[i+1])
            {
                end = j;
                std::vector<std::string>::const_iterator first =
                        v1.begin() + start + 1;
                std::vector<std::string>::const_iterator last =
                        v1.begin() + end;
                std::vector<std::string> newVec(first, last);
                if (buildInfos[i]=="INPUT-ALPHABET")
                {
                    InputAlphabetVector = newVec;
                }
                else if (buildInfos[i]=="TAPE-ALPHABET")
                {
                    TapeAlphabetVector = newVec;
                }
                else if (buildInfos[i]=="BLANK-SYMBOL")
                {
                    BlankSymbolVector = newVec;
                }
                else if (buildInfos[i]=="STATES")
                {
                    StatesVector = newVec;
                }
                else if (buildInfos[i]=="START-STATE")
                {
                    StartStateVector = newVec;
                }
                else if (buildInfos[i]=="ACCEPT-STATE")
                {
                    AcceptStateVector = newVec;
                }
                else if (buildInfos[i]=="REJECT-STATE")
                {
                    RejectStateVector = newVec;
                }
                else if (buildInfos[i]=="TRANSITIONS")
                {
                    TransitionsVector = newVec;
                }
                else if (buildInfos[i]=="INPUT-STRING")
                {
                    InputStringVector = newVec;
                }

            }
        }
    }
     auto tm1  = TuringMachine(InputAlphabetVector,
    TapeAlphabetVector,
    BlankSymbolVector,
    StatesVector,
    StartStateVector,
    AcceptStateVector,
    RejectStateVector,
    TransitionsVector,
    InputStringVector);

    tm1.testTheString();


    return 0;
}
