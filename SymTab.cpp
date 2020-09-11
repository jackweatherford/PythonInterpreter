#include <iostream>
#include "SymTab.hpp"
#include "Globals.hpp"

SymTab::SymTab() {
    i = 0;
    std::map<std::string, TypeDescriptor*> firstSymTab;
    symTabs.push_back(firstSymTab);
}

bool SymTab::isDefined(std::string vName) {
    return symTabs[i].find(vName) != symTabs[i].end();
}

void SymTab::setValueFor(std::string vName, TypeDescriptor* td) {
    // Define a variable by setting its initial value.
	if (verbose) {
		std::cout << vName << " <- ";
		td->print();
		std::cout << std::endl;
	}
    if (isDefined(vName)) {
        delete symTabs[i].find(vName)->second;
        symTabs[i].erase(vName);
    }
    symTabs[i][vName] = td;
}

TypeDescriptor* SymTab::getValueFor(std::string vName) {
    if( ! isDefined(vName)) {
		std::cout << "SymTab::getValueFor: " << vName << " has not been defined.\n";
		exit(1);
    }
	if (verbose) {
		std::cout << "SymTab::getValueFor: " << vName << " contains "; 
		symTabs[i].find(vName)->second->print();
		std::cout << std::endl;
	}

    return symTabs[i].find(vName)->second;
}

void SymTab::print() {
    for(auto it = symTabs[i].begin(); it != symTabs[i].end(); it++) {
        std::cout << it->first << " = ";
        it->second->print();
        std::cout << std::endl;
    }
}

void SymTab::openScope(std::map<std::string, TypeDescriptor*> newSymTab) {
    symTabs.push_back(newSymTab);
    i++;
}

void SymTab::closeScope() {
    symTabs.pop_back();
    i--;
}

void SymTab::storeReturnValue(TypeDescriptor *val) {
    returnVal = val;
}

TypeDescriptor* SymTab::getReturnValue() {
    return returnVal;
}
