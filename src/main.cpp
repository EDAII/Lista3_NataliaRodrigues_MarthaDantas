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
void startProgram();
void sort_1();
void sort_2();
vector<pair<int, char>> insertionSort(vector<pair<int, char>> word);
vector<pair<int, char>>  countingSort(vector<pair<int, char> > word);
void saveOrdenedWord(vector<pair<int, char>> word, string filename);
void finishProgram();
int countOrdened(string filename);
void saveDisorderedWords(string filename, int counter);
void writeBegin(FILE *fp);
void writeEnd(FILE *fp, int counter);
void savedWords(vector<pair<int,char>> word);

void createHTMLFiles(string directory, string ordened_file, string disordered_file);
void createOrdenedFile(string directory, string filename);
void createDisorderedFile(string directory, string filename);
FILE *generateHTML(string t);
void closeHTML(FILE *fp);


vector<vector<pair<int, char>>> saved_words;
float time_execution;
int position;

string iof = "../doc/insertion/ordened_words.txt";
string idf = "../doc/insertion/disordered_words.txt";
string cof = "../doc/counting/ordened_words.txt";
string cdf = "../doc/counting/disordered_words.txt";

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

    startProgram();
}

void startProgram() {
    remove(iof.c_str());
    remove(idf.c_str());
    remove(cof.c_str());
    remove(cdf.c_str());

    pid_t insertion = fork();

    if(insertion == 0) {
        sort_1();
    }
    else {
        pid_t counting = fork();

        if(counting == 0){
            sort_2();
        }
        else {
            sleep(time_execution);
            kill(insertion, SIGTERM);
            kill(counting, SIGTERM);
            finishProgram();
        }
    }
}

void sort_1() {
    for(unsigned int i = position; i < saved_words.size(); i++) {
        saveOrdenedWord(insertionSort(saved_words[i]), iof);
    }
}

void sort_2() {
    for(unsigned int i = position; i < saved_words.size(); i++) {
        saveOrdenedWord(insertionSort(saved_words[i]), cof);
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

vector<pair<int, char>> countingSort(vector<pair<int, char>> word) {
    vector<pair<int, char>> ordened_word(word.size());
    vector<int> counting_vetor((word.size()+1),0);

    for(unsigned int i = 0; i < word.size(); i++) {
        ++counting_vetor[word[i].first];
    }

    for(unsigned int i = 1; i <= word.size(); i++) {
        counting_vetor[i] += counting_vetor[i - 1];
    } 

    for(int i = word.size() - 1; i >= 0; i--) {
        ordened_word[counting_vetor[word[i].first] - 1] = word[i];
        --counting_vetor[word[i].first];
    }

    return ordened_word;
}

void saveOrdenedWord(vector<pair<int, char>> word, string filename) {
    
    if(word.size() > 0) {
        FILE *fp = fopen(filename.c_str(), "a+");

        if(fp == NULL)
            cout << "\nOH MEU DEUS\n";

        for(unsigned int i = 0; i < word.size(); i++) {
            fprintf(fp, "%c", word[i].second);
        }
        fprintf(fp, "\n");

        fclose(fp);
    }
}

void finishProgram() {

    cout << "\nCreating files..." << endl;

    int counter_insertion = countOrdened(iof);
    int counter_counting = countOrdened(cof);

    saveDisorderedWords(idf, counter_insertion);
    saveDisorderedWords(cdf, counter_counting);

    createHTMLFiles("insertion", iof, idf);
    createHTMLFiles("counting", cof, cdf);

    system("clear");

    cout << "\n\n================================ INSERTION SORT ================================\n\n";

    if(counter_insertion > 0) {
        cout << "\n" << counter_insertion - 1 << " words were ordened!\n\n";
        cout << "Open '" << iof << "' on your browser to see the ordened words.\n";
        cout << "Open '" << idf << "' on your browser to see the disordered words.\n\n";
    } else {
        cout << "\nOps... no words were ordened.\n\n";
        cout << "Open '" << idf << "' on your browser to see the disordered words.\n\n";
        remove(iof.c_str());
    }

    cout << "\n\n================================ COUNTING SORT ================================\n\n";

    if(counter_counting > 0) {
        cout << "\n" << counter_counting - 1 << " words were ordened!\n\n";
        cout << "Open '" << cof << "' on your browser to see the ordened words.\n";
        cout << "Open '" << cdf << "' on your browser to see the disordered words.\n\n";
    } else {
        cout << "\nOps... no words were ordened.\n\n";
        cout << "Open '" << cdf << "' on your browser to see the disordered words.\n\n";
        remove(cdf.c_str());
    }
}

int countOrdened(string filename) {
    FILE *wd = fopen(filename.c_str(), "a+");
    int counter = 0;
    char a;

    if(wd == NULL)
        cout << "\nAQUI\n";

    while(!feof(wd)) {
        fscanf(wd, "%c", &a);

        if(a == '\n')
            counter++;
    }

    fclose(wd);

    return counter;
}

void saveDisorderedWords(string filename, int counter) {
    FILE *fp = fopen(filename.c_str(), "a+");

    writeBegin(fp);
    writeEnd(fp, counter);

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

void writeEnd(FILE *fp, int counter) {

    if(counter > 0)
        counter--;

    for(unsigned int i = position + counter; i < saved_words.size(); i++) {
        for(unsigned int j = 0; j < saved_words[i].size(); j++){
            fprintf(fp, "%c", saved_words[i][j].second);
        }
        fprintf(fp, "\n");
    }
}

void createHTMLFiles(string directory, string ordened_file, string disordered_file) {
    createOrdenedFile(directory, ordened_file);
    createDisorderedFile(directory, disordered_file);
}

void createOrdenedFile(string directory, string filename) {
    FILE *fp = generateHTML(directory + "/ordened_words");
    FILE *ord = fopen(filename.c_str(), "r");
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

    remove(filename.c_str());
}

void createDisorderedFile(string directory, string filename) {
    FILE *fp = generateHTML(directory + "/disordered_words");
    FILE *dis = fopen(filename.c_str(), "r");
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

    remove(filename.c_str());
}

FILE *generateHTML(string t) {
    string filename = "../doc/" + t + ".html";

    FILE *fp = fopen(filename.c_str(), "w+");

    fprintf(fp, "<!DOCTYPE html>\n");

    fprintf(fp, "<html>\n<head>\n");
    fprintf(fp, "<title>%s</title>\n", t.c_str());
    fprintf(fp, "</head>\n");

    fprintf(fp, "\n<body>\n");

    return fp;
}

void closeHTML(FILE *fp) {
    fprintf(fp, "\n</body>\n</html>\n");
    fclose(fp);
}