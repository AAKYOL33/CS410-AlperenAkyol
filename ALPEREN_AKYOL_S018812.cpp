#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <list>
using namespace std;


/*
instead of using std::find from #include <algorithm>,
use the function below to check if an element is in a container (vector or list),
if it exists the number returned will be larger than 0,
if it doesnt exist it will be 0.
*/
template<typename Container,typename T>
int doesExists(Container& container, T& element){
    auto seenTimes = 0;
    for (auto& entry : container)
    {
        if (entry == element)
        {
            seenTimes ++;
        }
    }
    return seenTimes;
}

int isItInString(std::string& main, char& search){
    auto seenTimes = 0;
    for (auto& character: main) {
        if (character == search){
            seenTimes ++;
        }
    }
    return seenTimes;
}

std::string stateMerger(std::string& main, std::string& s2){
    auto stringToReturn = main;
    for (auto& element: s2) {
        auto foundTimes = isItInString(main, element);
        if (foundTimes == 0){
            stringToReturn.append(std::string(1, element));
        }
    }
    return stringToReturn;
}

// takes the transitions vector,
// takes a state and an input and gives where the state goes to with that input
std::list<std::string> whereDoesItGo(std::vector<std::string>& v, char state, char input){
    auto stateToReturn = std::list<std::string>();
    for (auto& line: v) {
        if (line[0] == state && line[2] == input){
            stateToReturn.push_back(std::string(1,line[4]));
        }
    }
    return stateToReturn;
}

int findFinalState(std::string& main, std::string& final){

    auto seenTimes = 0;
    for (auto& entry : main)
    {
        if (std::string(1,entry) == final)
        {
            seenTimes ++;
        }
    }
    return seenTimes;

}




struct NFA
{
    std::vector<std::string> alphabet;
    std::vector<std::string> states;
    std::vector<std::string> start;
    std::vector<std::string> final;
    std::vector<std::string> transitions;

    NFA(std::vector<std::string> alphabet,
        std::vector<std::string> states,
        std::vector<std::string> start,
        std::vector<std::string> final,
        std::vector<std::string> transitions)
            :alphabet(alphabet),states(states),start(start),
             final(final),transitions(transitions){}

};


struct DFA
{
    std::vector<std::string> alphabet;
    std::list<std::string> states;
    std::vector<std::string> start;
    std::vector<std::string> final;
    std::vector<std::string> transitions;

    DFA(NFA& nfa){
        std::map<std::string,std::string> tempTransitionsDict;
        std::map<std::string,std::string> tempTransitionsDict2;
        std::list<std::string> tempStates;
        auto copyOfTransitions = nfa.transitions;
        std::list<std::string> statesToCombine;
        std::map<std::string,std::string> inputToState;
        this->alphabet = nfa.alphabet;
        this->start = nfa.start;
        for(auto& entry: nfa.transitions){
            if(this->start[0][0] == entry[0]){
                auto firstHalf = std::string(1, entry[0]);
                firstHalf.append(" ");
                firstHalf.append(std::string(1, entry[2]));
                auto nextLine = *(&entry + 1);
                inputToState[std::string(1,entry[2])] = std::string(1,entry[4]);
                for (auto& elem: inputToState) {
                    tempTransitionsDict[firstHalf] = elem.second;
                    statesToCombine.push_back(elem.second);
                }
                auto size = statesToCombine.size();
                if (size > 1){
                    std::string temp;
                    for(auto& entry2:statesToCombine){
                        temp.append(entry2);
                        //std::cout<<temp<<std::endl;
                    }
                    auto existsVal = doesExists(tempStates,temp);
                    if (existsVal == 0){
                        tempTransitionsDict[firstHalf] = temp;
                        tempStates.push_back(temp);
                    }
                }
                // delete loop to clear temporary values
                if (std::string(1,nextLine[2]) != std::string(1,entry[2]))
               {
                   if (std::string(1,nextLine[0]) == std::string(1,entry[0])){
                       // copy values before cleaning...
                       for(auto& state: statesToCombine){
                           auto existVal = doesExists(tempStates,state);
                           if (existVal == 0){
                               tempStates.push_back(state);
                           }
                       }
                       inputToState.clear();
                       statesToCombine.clear();
                   }
               }
            }
        }
        inputToState.clear();
        statesToCombine.clear();
        this->states = tempStates;
        for(auto& entry: tempTransitionsDict){
            //std::cout<<entry.first<<" => "<<entry.second<<std::endl;
            auto valueToAdd = entry.first;
            valueToAdd.append(" ");
            valueToAdd.append(entry.second);
            this->transitions.push_back(valueToAdd);

        }
        tempStates.remove(this->start[0]);
        for(auto& newStates: tempStates){
            auto currentState = newStates;
            if (currentState != this->start[0]){
                std::multimap<std::string,std::string> multiInputToState;
                for(auto& singleState: newStates){
                    auto singleStateGoesTo = std::string();
                    auto firstHalf = std::string();
                    for(auto& input: this->alphabet) {
                        auto goesToState = whereDoesItGo(copyOfTransitions,
                                                         singleState, input[0]);

                        firstHalf = std::string(1, singleState);
                        firstHalf.append(" ");
                        firstHalf.append(input);
                        for(auto& xyz: goesToState){
                            singleStateGoesTo.append(xyz);
                            /*std::cout << singleState << " goes to: "
                                 << xyz
                                 << " with: " << input << std::endl;*/
                        }
                        if (!singleStateGoesTo.empty()){
                            multiInputToState.insert(pair<std::string,std::string>(firstHalf,singleStateGoesTo));
                        }
                        singleStateGoesTo = "";
                    }
                }
                for (auto& input2: this->alphabet) {
                    auto stateForInput = std::string();
                    for(auto& entry: multiInputToState){
                        if (entry.first.back() == input2[0]){
                            //string merger
                            stateForInput = stateMerger(stateForInput, entry.second);
                        }
                    }
                    //std::cout<<stateForInput<<std::endl;
                    if (!stateForInput.empty()){
                        auto foundOrNot = doesExists(tempStates, stateForInput);
                        if (foundOrNot == 0) {
                            tempStates.push_back(stateForInput);
                        }
                        auto firstHalf = std::string();
                        firstHalf.append(currentState);
                        firstHalf.append(" ");
                        firstHalf.append(input2);
                        tempTransitionsDict2[firstHalf] = stateForInput;
                    }

                }

            }
        }
        // print to test
        for(auto& entry: tempStates){
            auto foundTimes = doesExists(this->states, entry);
            if(foundTimes == 0){
                this->states.push_back(entry);
            }
            //std::cout<<entry<<std::endl;
        }
        for(auto& finalStates: nfa.final){
            for(auto& entry: this->states){
                auto foundTimes = findFinalState(entry, finalStates);
                if (foundTimes !=0 ){
                    this->final.push_back(entry);
                }
            }
        }
        for(auto& entry: tempTransitionsDict2){
            //std::cout<<entry.first<<" => "<<entry.second<<std::endl;
            auto valueToAdd = entry.first;
            valueToAdd.append(" ");
            valueToAdd.append(entry.second);
            this->transitions.push_back(valueToAdd);

        }

        // convert the NFA to DFA
    }
    void DFAprint(){
        std::cout<<"ALPHABET"<<std::endl;
        for(auto& elem: this->alphabet){
            std::cout<<elem<<std::endl;
        }
        std::cout<<"STATES"<<std::endl;
        for(auto& elem: this->states){
            std::cout<<elem<<std::endl;
        }
        std::cout<<"START"<<std::endl;
        for(auto& elem: this->start){
            std::cout<<elem<<std::endl;
        }
        std::cout<<"FINAL"<<std::endl;
        for(auto& elem: this->final){
            std::cout<<elem<<std::endl;
        }
        std::cout<<"TRANSITIONS"<<std::endl;
        for(auto& elem: this->transitions){
            std::cout<<elem<<std::endl;
        }
        std::cout<<"END"<<std::endl;
    }

};


int main()
{
    std::ifstream file("NFA1.txt");
    std::string str;

    // information vectors for the NFA
    std::vector<std::string> NFAAlphabetVector;
    std::vector<std::string> NFAStatesVector;
    std::vector<std::string> NFAStartVector;
    std::vector<std::string> NFAFinalVector;
    std::vector<std::string> NFATransitionsVector;

    std::vector<std::string> v1;
    while (std::getline(file, str))
    {
        v1.push_back(str);
    }
    std::string buildInfos[6] = {"ALPHABET","STATES","START","FINAL","TRANSITIONS","END"};
    for (int i = 0; i < 5; i++)
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
                if (buildInfos[i]=="ALPHABET")
                {
                    NFAAlphabetVector = newVec;
                }
                else if (buildInfos[i]=="STATES")
                {
                    NFAStatesVector = newVec;
                }
                else if (buildInfos[i]=="START")
                {
                    NFAStartVector = newVec;
                }
                else if (buildInfos[i]=="FINAL")
                {
                    NFAFinalVector = newVec;
                }
                else if (buildInfos[i]=="TRANSITIONS")
                {
                    NFATransitionsVector = newVec;
                }

            }
        }
    }
    auto nfa = NFA(NFAAlphabetVector,NFAStatesVector,
                   NFAStartVector,NFAFinalVector,NFATransitionsVector);
    auto dfa = DFA(nfa);
    dfa.DFAprint();



    return 0;

    
}