#include <iostream>
#include <unistd.h>
#include <algorithm>
#include <vector>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <cstring>

using namespace std;

FILE * openFile(FILE * file);
void readWords(FILE * file);
void randomChacarter(vector<pair<int,char>> word);
void getInit();
void startProgram(int algorithm);
void doWhatChildDo(int algorithm);
vector<pair<int, char>> insertionSort(vector<pair<int, char>> word);
void saveOrdenedWord(vector<pair<int, char>> word);
void finishProgram();
void countOrdened();
void saveDisorderedWords();
void writeBegin(FILE *fp);
void writeEnd(FILE *fp);
void savedWords(vector<pair<int,char>> word);
vector<pair<int, char>>  countingSort(vector<pair<int, char> > word);

void createHTMLFiles();
void createOrdenedFile();
void createDisorderedFile();
FILE *generateHTML(string t);
void closeHTML(FILE *fp);
int chooseSortAgorithm();


vector<vector<pair<int, char>>> saved_words;
float time_execution;
int position;
int counter = 0;

int main() {
    FILE * file;
  
    file = openFile(file);
    readWords(file);
    getInit();

    return 0;
}

FILE * openFile(FILE * file) {
    file = fopen("../doc/words.txt", "r"); 
    
    if(file == NULL) {
        cout << "Error opening File, finishing execution..." << endl;
        exit(0);
    }
    else {
        // nothind to do.
    }

    return file;
}

void readWords(FILE * file) {
    vector<pair<int,char>> word;
    char aux; 
    int i = 0;
    
    while(fscanf(file,"%c", &aux) != EOF) {
        if(aux != '\n') {
            word.push_back(make_pair(i,aux));
            i++;
        }
        else {
            randomChacarter(word);
            i = 0;
            word.erase(word.begin(), word.end());
        }
        
    }
    randomChacarter(word);
    fclose(file);
}

void randomChacarter(vector<pair<int,char>> word) {
    if(word.size() > 0){
        random_shuffle(word.begin(), word.end());
        savedWords(word);
    }
}

void savedWords(vector<pair<int,char>> word) {
    saved_words.push_back(word);
}

void getInit() {
    cout << "\nEnter time for program execution: ";
    cin >> time_execution;

    cout << "Enter start position for programa execution: ";
    cin >> position;

    startProgram(chooseSortAgorithm());
}

void startProgram(int algorithm) {
    remove("../doc/ordened_words.txt");
    remove("../doc/disordered_words.txt");

    pid_t child = fork();

    if(child == 0) {
        doWhatChildDo(algorithm);
    }
    else {
        sleep(time_execution);
        kill(child, SIGTERM);
        finishProgram();
    }
}

void doWhatChildDo(int algorithm) {
    for(unsigned int i = position; i < saved_words.size(); i++) {
        if(algorithm == 1) {
            saveOrdenedWord(insertionSort(saved_words[i]));
        }
        else if (algorithm == 2) {
            saveOrdenedWord(countingSort(saved_words[i]));  
        }
        
    }
}

vector<pair<int, char>> insertionSort(vector<pair<int, char>> word) {
    vector<pair<int, char>> ordened_word = word;
    pair<int, char> key;
    unsigned int i;
    int j;
    
    for (i = 1; i < ordened_word.size(); i++) { 
        key = ordened_word[i]; 
        j = i - 1; 
  
        while (j >= 0 && ordened_word[j].first > key.first) { 
            ordened_word[j+1] = ordened_word[j]; 
            j--; 
        } 
        ordened_word[j+1] = key; 
    } 

    return ordened_word;
}

void saveOrdenedWord(vector<pair<int, char>> word) {
    
    if(word.size() > 0) {
        FILE *fp = fopen("../doc/ordened_words.txt", "a+");

        for(unsigned int i = 0; i < word.size(); i++) {
            fprintf(fp, "%c", word[i].second);
        }
        fprintf(fp, "\n");

        fclose(fp);
    }
}

void finishProgram() {
    countOrdened();
    saveDisorderedWords();
    createHTMLFiles();

    if(counter > 0) {
        cout << "\n" << counter << " words were ordened!\n\n";
        cout << "Open '..doc/ordened_words.html' on your browser to see the ordened words.\n";
        cout << "Open '..doc/disordered_words.html' on your browser to see the disordered words.\n\n";
    } else {
        cout << "\nOps... no words were ordened.\n\n";
        cout << "Open '..doc/disordered_words.html' on your browser to see all disordered words.\n\n";
        remove("../doc/ordened_words.html");
    }
}

void countOrdened() {
    FILE *wd = fopen("../doc/ordened_words.txt", "a+");
    char a;

    while(!feof(wd)) {
        fscanf(wd, "%c", &a);

        if(a == '\n')
            counter++;
    }

    fclose(wd);
}

void saveDisorderedWords() {
    FILE *fp = fopen("../doc/disordered_words.txt", "a+");

    writeBegin(fp);
    writeEnd(fp);

    fclose(fp);
}

void writeBegin(FILE *fp) {
    for(int i = 0; i < position; i++) {
        for(unsigned int j = 0; j < saved_words[i].size(); j++) {
            fprintf(fp, "%c", saved_words[i][j].second);
        }
        fprintf(fp, "\n");
    }
}

void writeEnd(FILE *fp) {

    if(counter > 0)
        counter --;

    for(unsigned int i = position + counter; i < saved_words.size(); i++) {
        for(unsigned int j = 0; j < saved_words[i].size(); j++){
            fprintf(fp, "%c", saved_words[i][j].second);
        }
        fprintf(fp, "\n");
    }
}

void createHTMLFiles() {
    createOrdenedFile();
    createDisorderedFile();
}

void createOrdenedFile() {
    FILE *fp = generateHTML("ordened_words");
    FILE *ord = fopen("../doc/ordened_words.txt", "r");
    char a;

    fprintf(fp, "<p style=\"color:green\">");
    while(!feof(ord)) {
        fscanf(ord, "%c", &a);

        if(a != '\n'){
            fprintf(fp, "%c", a);
        }
        else {
            fprintf(fp, "</p>\n");
            fprintf(fp, "<p style=\"color:green\">");
        }
    }
    fprintf(fp, "</p>\n");

    closeHTML(fp);
    fclose(ord);

    remove("../doc/ordened_words.txt");
}

void createDisorderedFile() {
    FILE *fp = generateHTML("disordered_words");
    FILE *dis = fopen("../doc/disordered_words.txt", "r");
    char a;

    fprintf(fp, "<p style=\"color:red\">");
    while(!feof(dis)) {
        fscanf(dis, "%c", &a);

        if(a != '\n'){
            fprintf(fp, "%c", a);
        }
        else {
            fprintf(fp, "</p>\n");
            fprintf(fp, "<p style=\"color:red\">");
        }
    }
    fprintf(fp, "</p>\n");

    closeHTML(fp);
    fclose(dis);

    remove("../doc/disordered_words.txt");
}

FILE *generateHTML(string t) {
    string n = "../doc/" + t + ".html";

    char fileName[n.size() + 1];
    strcpy(fileName, n.c_str());

    char title[t.size() + 1];
	strcpy(title, t.c_str());

    FILE *fp = fopen(fileName, "w+");

    fprintf(fp, "<!DOCTYPE html>\n");
    fprintf(fp, "<html>\n<head>\n");
    fprintf(fp, "<title>%s</title>\n", title);
    fprintf(fp, "</head>\n</html>\n");

    fprintf(fp, "\n<body>\n");

    return fp;
}

void closeHTML(FILE *fp) {
    fprintf(fp, "\n</body>\n");
    fclose(fp);
}

vector<pair<int, char>> countingSort(vector<pair<int, char> > word) {
    vector<pair<int, char> > ordened_word(word.size());
    vector<int> counting_vetor(word.size()+1,0);

    for(unsigned int i = 0; i <= word.size(); i++) {
       ++counting_vetor[word[i].first];
    }

    for(unsigned int i = 0; i < word.size(); i++) {
        int aux;
        aux = counting_vetor[i] + counting_vetor[i+1];
        counting_vetor[i+1] = aux;
    } 

    for(int i = word.size(); i >= 0; i--) {
        int position = counting_vetor[word[i].first];
        ordened_word[position] = word[i];
    }

    return ordened_word;
}

int chooseSortAgorithm() {
    int input;

    cout << "1 - Insertin Sort: " << "\n" << "2 - Counting Sort"<< endl;
    cout << "Escolha o algoritmo de ordenação:" << endl;
    cin >> input;

    return input;
}