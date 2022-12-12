#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <unordered_map>

using namespace std;



// helper function
int doesContain(std::string& main, char& search){
    auto seenTimes = 0;
    for (auto& character: main) {
        if (character == search){
            seenTimes ++;
        }
    }
    return seenTimes;
}
template<typename Container, typename T>
auto inContainer(Container& container, T& elem){
    auto seenTimes = 0;
    for (auto& character: container) {
        if (character == elem){
            seenTimes ++;
        }
    }
    return seenTimes;
}

int isItTerminal(std::vector<std::string>& main, char& search){
    auto seenTimes = 0;
    for (auto& character: main) {
        if (character == std::string(1,search)){
            seenTimes ++;
        }
    }
    return seenTimes;
}
template<typename Container>
auto doesPairExists(Container& container, std::pair<std::string,std::string>& p){
    auto seenTimes = 0;
    for(auto& entry: container){
        if(entry.first == p.first && entry.second == p.second){
            seenTimes ++;
        }
    }
    return seenTimes;
}
template<typename Container, typename T>
auto howManyNullables(Container& c, T& str){
    auto nullableNum = 0;
    for (auto& chr: str) {
        for(auto& nullables: c){
            if (chr == nullables[0]){
                nullableNum ++;
            }
        }
    }
    return nullableNum;
}
template<typename Container, typename T>
auto whichNullables(Container& c, T& str){
    Container nullableC;
    for (auto& chr: str) {
        for(auto& nullables: c){
            if (chr == nullables[0]){
                nullableC.push_back(nullables);
            }
        }
    }
    return nullableC;
}

struct CFG{
    std::string CFGStart;
    std::vector<std::string> CFGNonTerminalVector;
    std::vector<std::string> CFGTerminalVector;
    std::unordered_multimap<std::string , std::string> CFGRulesMap;

    CFG(std::string Start,std::vector<std::string> NonTerminalVector,
        std::vector<std::string> TerminalVector,std::vector<std::string> RulesVector)
            : CFGStart{Start}, CFGNonTerminalVector{NonTerminalVector},
              CFGTerminalVector{TerminalVector}{
        for(auto& elem: RulesVector){
            //cout<<elem<<endl;
            std::string delimiter = ":";
            std::string token = elem.substr(0, elem.find(delimiter));
            std::string goesTo = elem.substr(elem.find(delimiter));
            goesTo = goesTo.substr(1,goesTo.size());
            CFGRulesMap.insert(std::pair<std::string,std::string>(token,goesTo));
        }
    }
};

struct CNF{
    std::string CNFStart;
    std::vector<std::string> CNFNonTerminalVector;
    std::vector<std::string> CNFTerminalVector;
    std::vector<std::string> CNFRulesVectorPrint;
    std::unordered_multimap<std::string, std::string> CNFRulesMap;

    CNF(CFG& cfg):CNFStart{cfg.CFGStart},CNFNonTerminalVector{cfg.CFGNonTerminalVector},
                  CNFTerminalVector{cfg.CFGTerminalVector},CNFRulesMap{cfg.CFGRulesMap}{}

    // replace the start state with S0
    auto step1(){
        std::string newStart = "S0";
        CNFRulesMap.insert(std::pair<std::string,std::string>(newStart,CNFStart));
        //std::string tempRule = newStart + ":"+ CNFStart;
        CNFStart = newStart;
        CNFNonTerminalVector.insert(CNFNonTerminalVector.begin(),newStart);
    }
    // eliminate nullable variables
    auto step2(){
        // get the non-terminals that directly go to epsilon
        std::list<std::string> nullableList;
        char epsilonChar = 'e';
        for(auto& elem: CNFNonTerminalVector){
            for(auto& mapEntry: CNFRulesMap){
                if (mapEntry.first == elem){
                    auto numTimes = doesContain(mapEntry.second, epsilonChar);
                    if (numTimes > 0){
                        nullableList.push_back(elem);
                    }
                }
            }
        }
        // get the non-terminals that eventually lead to epsilon
        for(auto& tempEntry: CNFRulesMap){
            auto sz = tempEntry.second.size();
            auto numOfNulls = howManyNullables(nullableList,tempEntry.second);
            if(sz == numOfNulls){
                nullableList.push_back(tempEntry.first);
            }

        }
        // continue for nullable list, remove direct epsilon transitions
        // add rules without nullable chars
        std::unordered_multimap<std::string, std::string> tempCNFRulesMap;
        for(auto& mapEntry:CNFRulesMap){
            if (mapEntry.second[0] == epsilonChar){
                // do nothing null transition
            }
            // add the rest
            else {
                auto numOfTimes2 = inContainer(tempCNFRulesMap, mapEntry);
                if (numOfTimes2 == 0) {
                    tempCNFRulesMap.insert(std::pair<std::string, std::string>
                    {mapEntry.first, mapEntry.second});
                }
            }
        }
        // add new rules for nullables
        // A-> aBb ... A->ab
        CNFRulesMap = tempCNFRulesMap;
        for (auto& mapEntry: CNFRulesMap) {
            auto nullableSeenTimes = howManyNullables(nullableList, mapEntry.second);
            if (nullableSeenTimes == 1){
                for(auto& nullableChar: nullableList){
                    auto numOfTimes = doesContain(mapEntry.second,nullableChar[0]);
                    if (numOfTimes > 0){
                        std::string newRuleRHS;
                        for(auto& character: mapEntry.second){
                            if (character != nullableChar[0]){
                                newRuleRHS.append(std::string(1,character));
                            }
                        }
                        auto tmpPair = std::pair<std::string,std::string>{
                                mapEntry.first, newRuleRHS
                        };
                        auto numOfTimes2 = doesPairExists(CNFRulesMap,tmpPair);
                        if(numOfTimes2 == 0 && newRuleRHS != "" && newRuleRHS != mapEntry.first){
                            CNFRulesMap.insert(std::pair<std::string,std::string>{
                                    mapEntry.first, newRuleRHS
                            });
                        }
                    }
                }
            }
            // if amount of nullables is larger than 1
            else{
                auto nullablesInString = whichNullables(nullableList, mapEntry.second);
                // new string without one nullable A-> aBcAd == A-> acAd / A-> aBcd
                // check if the same nullable exists
                for(auto& nullableTemp: nullableList){
                    auto nullableSeenTimes2 = doesContain(mapEntry.second, nullableTemp[0]);
                    // if one nullable exist once
                    if(nullableSeenTimes2 == 1){
                        std::string newRuleRHS;
                        for(auto& character: mapEntry.second){
                            if (character != nullableTemp[0]){
                                newRuleRHS.append(std::string(1,character));
                            }
                        }
                        auto tmpPair = std::pair<std::string,std::string>{
                                mapEntry.first, newRuleRHS
                        };
                        auto numOfTimes2 = doesPairExists(CNFRulesMap,tmpPair);
                        if(numOfTimes2 == 0 && newRuleRHS != "" && newRuleRHS != mapEntry.first){
                            CNFRulesMap.insert(std::pair<std::string,std::string>{
                                    mapEntry.first, newRuleRHS
                            });
                        }
                    }
                    // same nullable exists more than 1
                    else{
                        std::size_t found = mapEntry.second.find(nullableTemp);
                        if (found!=std::string::npos){
                            std::string newRule1 = mapEntry.second;
                            newRule1.replace(found, nullableTemp.length(), "");
                            auto tmpPair = std::pair<std::string,std::string>{
                                    mapEntry.first, newRule1
                            };
                            auto numOfTimes2 = doesPairExists(CNFRulesMap,tmpPair);
                            if(numOfTimes2 == 0 && newRule1 != "" && newRule1 != mapEntry.first){
                                CNFRulesMap.insert(std::pair<std::string,std::string>{
                                        mapEntry.first, newRule1
                                });
                            }
                        }
                        for (int i = 1; i < nullableSeenTimes2; ++i) {
                            found = mapEntry.second.find(nullableTemp, found + i);
                            if (found!=std::string::npos){
                                std::string newRule1 = mapEntry.second;
                                newRule1.replace(found, nullableTemp.length(), "");
                                auto tmpPair = std::pair<std::string,std::string>{
                                        mapEntry.first, newRule1
                                };
                                auto numOfTimes2 = doesPairExists(CNFRulesMap,tmpPair);
                                if(numOfTimes2 == 0 && newRule1 != "" && newRule1 != mapEntry.first){
                                    CNFRulesMap.insert(std::pair<std::string,std::string>{
                                            mapEntry.first, newRule1
                                    });
                                }
                            }

                        }
                    }
                }
            // remove all nullables and add that rule...
                for(auto& nullableChar: nullableList){
                    auto numOfTimes = doesContain(mapEntry.second,nullableChar[0]);
                    if (numOfTimes > 0){
                        std::string newRuleRHS;
                        for(auto& character: mapEntry.second){
                            if (character != nullableChar[0]){
                                newRuleRHS.append(std::string(1,character));
                            }
                        }
                        auto tmpPair = std::pair<std::string,std::string>{
                                mapEntry.first, newRuleRHS
                        };
                        auto numOfTimes2 = doesPairExists(CNFRulesMap,tmpPair);
                        if(numOfTimes2 == 0 && newRuleRHS != "" && newRuleRHS != mapEntry.first){
                            CNFRulesMap.insert(std::pair<std::string,std::string>{
                                    mapEntry.first, newRuleRHS
                            });
                        }
                    }
                }

            }
        }
        auto seenTimes = inContainer(nullableList, CNFStart);
        if (seenTimes > 0){
            CNFRulesMap.insert(std::pair<std::string,std::string>{
                    CNFStart,std::string(1,epsilonChar)
            });
        }
    }
    auto step3(){
        // check if any RHS is a unit rule
        // if it is fetch the rules for that unit rule
        // add the rules with the correct non-terminal
        std::unordered_multimap<std::string, std::string> tempCNFRulesMap;
        for(auto& mapEntry: CNFRulesMap){
            for(auto& nonTerminal: CNFNonTerminalVector){
                auto currentRHS = mapEntry.second;
                if (currentRHS == nonTerminal){
                    auto val = CNFRulesMap.equal_range(nonTerminal);
                    for (auto it = val.first; it != val.second; ++it) {
                        auto nonTerminalCheck = inContainer(CNFNonTerminalVector, it->second);
                        if (nonTerminalCheck == 0){
                            auto tmpPair = std::pair<std::string,std::string>{
                                    mapEntry.first, it->second
                            };
                            auto numOfTimes = doesPairExists(tempCNFRulesMap,tmpPair);
                            if(numOfTimes == 0){
                                tempCNFRulesMap.insert(std::pair<std::string,std::string>{
                                        mapEntry.first, it->second
                                });
                            }
                        }
                    }
                }
            }
        }
        // add rules to tempMap that do not have unit rules
        for(auto& entry: CNFRulesMap){
            auto nonTerminalCheck = inContainer(CNFNonTerminalVector, entry.second);
            if (nonTerminalCheck == 0){
                tempCNFRulesMap.insert(std::pair<std::string,std::string>{
                        entry.first, entry.second
                });
            }
        }
        CNFRulesMap = tempCNFRulesMap;
    }
    std::unordered_multimap<std::string, std::list<std::string>> copyCNFRulesMap;
    auto step4(){
        // ensure all RHS is one terminal or only non-terminals or S0->e
        // create replacement non-terminals for terminals: U1 -> a
        std::map<std::string,std::string> terminalReplacementMap;
        auto sz = CNFTerminalVector.size();
        for (int i = 1; i <= sz ; ++i) {
            std::string terminalReplacementString = "U";
            // to_string() is to convert int to string
            terminalReplacementString.append(std::to_string(i));
            terminalReplacementMap.insert(std::pair<std::string,std::string>{
                    CNFTerminalVector[i-1],terminalReplacementString
            });
        }
        // check if RHS has more then one symbol
        // and if some replace the terminal ones with their replacements
        for (auto& mapEntry: CNFRulesMap) {
            std::list<std::string> copyList;
            if (mapEntry.second.size() > 1){
                std::string newRHS;
                //cout<<mapEntry.first<<":"<<mapEntry.second<<endl;
                for (auto& element: mapEntry.second) {
                    //cout<<element<<endl;
                    auto numOfTimes = isItTerminal(CNFTerminalVector, element);
                    if (numOfTimes > 0){
                        auto equivalentNonTerminal = terminalReplacementMap[std::string(1,element)];
                        //cout<<element<<":"<<equivalentNonTerminal<<endl;
                        newRHS.append(equivalentNonTerminal);
                        copyList.push_back(equivalentNonTerminal);
                        auto tempPair = std::pair<std::string,std::string>{
                                equivalentNonTerminal, std::string(1,element)};
                        auto ifInRules = doesPairExists(CNFRulesMap, tempPair);
                        if (ifInRules == 0){
                            CNFRulesMap.insert(tempPair);
                        }
                        auto seenTimes = inContainer(CNFNonTerminalVector, equivalentNonTerminal);
                        if (seenTimes == 0){
                            CNFNonTerminalVector.push_back(equivalentNonTerminal);
                        }
                    }else{
                        newRHS.append(std::string(1,element));
                        copyList.push_back(std::string(1,element));
                    }
                }
                copyCNFRulesMap.insert(std::pair<std::string,std::list<std::string>>{
                   mapEntry.first, copyList
                });
                mapEntry.second = newRHS;
            }else {
                for (auto &entry: mapEntry.second) {
                    copyList.push_back(std::string(1,entry));
                }
                copyCNFRulesMap.insert(std::pair<std::string,std::list<std::string>>{
                        mapEntry.first, copyList
                });
            }
        }
    }
    auto step5(){
        // if non-terminals in th RHS is larger than 3,
        // create a new non-termianl that goes to the first two non-terminals
        std::map<std::string,std::string> nonTerminalReplacementMap;
        std::unordered_multimap<std::string, std::string> tempCNFRulesMap;
        std::vector<std::string> toBeReplacedVector;
        for(auto& mapEntry: copyCNFRulesMap){
            auto tempControlNumber = 0;
            if(mapEntry.second.size() >= 3){
                std::string firstTwoChars;
                for (auto& entry: mapEntry.second) {
                    if (tempControlNumber < 2) {
                        firstTwoChars.append(entry);
                        tempControlNumber++;
                        if (tempControlNumber == 2){
                            tempControlNumber = 0;
                            auto seenTimes = inContainer(toBeReplacedVector, firstTwoChars);
                            if (seenTimes == 0){
                                toBeReplacedVector.push_back(firstTwoChars);
                            }
                            firstTwoChars = "";
                        }
                    }
                }
            }
        }
        // create replacement rules
        auto sz = toBeReplacedVector.size();
        for (int i = 1; i <= sz; ++i) {
            std::string nonTerminalReplacementString = "Y";
            nonTerminalReplacementString.append(std::to_string(i));
            nonTerminalReplacementMap.insert(std::pair<std::string,std::string>{
                toBeReplacedVector[i-1],nonTerminalReplacementString
            });
            CNFNonTerminalVector.push_back(nonTerminalReplacementString);
        }
        // add replacement rules to rules and non-terminals
        for(auto& element: nonTerminalReplacementMap){
            CNFRulesMap.insert(std::pair<std::string,std::string>{
                element.second, element.first
            });
        }
        // replace
        for(auto& entry: CNFRulesMap){
            for (auto& tempVector: toBeReplacedVector) {
                if(entry.second.size() > tempVector.size()) {
                    size_t location = entry.second.find(tempVector);
                    if (location != std::string::npos) {
                        entry.second.replace(location, tempVector.length(), nonTerminalReplacementMap[tempVector]);
                    }
                }
            }
        }
    }

    auto removeNonReachables(){
        std::list<std::string> nonReachablesList;
        std::vector<std::string> tempVector;
        for(auto& element: CNFNonTerminalVector){
            if(element == CNFStart){
                tempVector.push_back(element);
            }else{
                auto seenTimes = 0;
                for (auto& mapEntry: CNFRulesMap) {
                    auto numTimes = doesContain(mapEntry.second, element[0]);
                    seenTimes += numTimes;
                }
                if (seenTimes == 0){
                    CNFRulesMap.erase(element);
                }
                else{
                    tempVector.push_back(element);
                }
            }
        }
        CNFNonTerminalVector = tempVector;
    }
    auto sortCNF(){
        // this method is used to sort rules based on the order of the non-terminals
        for(auto& nonTerminal: CNFNonTerminalVector){
            auto its = CNFRulesMap.equal_range(nonTerminal);
            for (auto it = its.first; it != its.second; it++) {
                std::string toAddString = it->first;
                toAddString.append(":");
                toAddString.append(it->second);
                CNFRulesVectorPrint.push_back(toAddString);
            }
        }
    }
    auto cnfPrint(){
        std::cout<<"NON-TERMINAL"<<std::endl;
        for(auto& element: CNFNonTerminalVector){
            std::cout<<element<<endl;
        }
        std::cout<<"TERMINAL"<<std::endl;
        for(auto& element: CNFTerminalVector){
            std::cout<<element<<endl;
        }
        std::cout<<"RULES"<<std::endl;
        for(auto& element: CNFRulesVectorPrint){
            std::cout<<element<<endl;
        }
        std::cout<<"START"<<std::endl;
        std::cout<<CNFStart<<std::endl;
    }
    auto operator()(){
        // call methods
        step1();
        step2();
        auto sz = CNFNonTerminalVector.size();
        for (int i = 0; i < sz ; ++i) {
            step3();
        }
        step4();
        step5();
        removeNonReachables();
        sortCNF();
    }
};
int main()
{
    std::ifstream file("G1.txt");
    std::string str;

    // information vectors for the NFA
    std::vector<std::string> InputNonTerminalVector;
    std::vector<std::string> InputTerminalVector;
    std::vector<std::string> InputRulesVector;


    std::vector<std::string> v1;
    while (std::getline(file, str))
    {
        v1.push_back(str);
    }
    auto InputStart = v1.back();
    std::string buildInfos[4] = {"NON-TERMINAL","TERMINAL",
                                 "RULES","START"};
    for (int i = 0; i < 3; i++)
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
                auto first =
                        v1.begin() + start + 1;
                auto last =
                        v1.begin() + end;
                std::vector<std::string> newVec(first, last);
                if (buildInfos[i]=="NON-TERMINAL")
                {
                    InputNonTerminalVector = newVec;
                }
                else if (buildInfos[i]=="TERMINAL")
                {
                    InputTerminalVector = newVec;
                }
                else if (buildInfos[i]=="RULES")
                {
                    InputRulesVector = newVec;
                }
                else if (buildInfos[i]=="START")
                {

                }
            }
        }
    }
    auto cfg1 = CFG(InputStart,InputNonTerminalVector,
                    InputTerminalVector,InputRulesVector);
    auto cnf1 = CNF(cfg1);
    cnf1();
    cnf1.cnfPrint();

    return 0;


}
