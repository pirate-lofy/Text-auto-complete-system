#include<iostream>
#include<cstring>
#include<queue>
#include<fstream>
#include<sstream>
#include<algorithm>
#include<iterator>

using namespace std;

const int alphaSize=26;
short valOfchar(char c){return c-'a';}

struct node{
  int frq;
  node*children[alphaSize];

  node(){
    memset(children,0,sizeof children);//to initialize all the pointer to null
    frq=0;
  }
};

struct item{
  string word;
  int frq;

  item(string s,int f){
    word=s;
    frq=f;
  }
  bool operator < (item b)const{return frq<b.frq;}
};

struct trie{
  node*root=new node();
  priority_queue<item> words; // this queue to store all prediction based on the frequency of existance

  // if it's already stored, just increment it's frq
  bool exist(string word){
    node*ptr=root;
    for(short i=0; i<word.length(); i++){
      short index=valOfchar(word[i]);
      if(ptr->children[index])
        ptr=ptr->children[index];
      else return false;
    }
    if(ptr->frq){
      ptr->frq++;
      return true;
    }
    return false;
  }



  void display(short n){
    cout<<"----------the most relevant words are----------\n";
    if(n>words.size())n=words.size();
    for(short i=0; i<n; i++){
        cout<<words.top().word<<endl;
        words.pop();
    }
  }

  void addNode(string word){
    transform(word.begin(),word.end(),word.begin(),::tolower); // transform all chars to lower case
    if(exist(word))return;
    node*ptr=root; // to start from the top of the tree

    for(short i=0; i<word.length(); i++){
      short index=valOfchar(word[i]);

      if(!ptr->children[index])
        ptr->children[index]=new node(); // create new room fr a new letter
      ptr=ptr->children[index];// move to the next char

      if(i==word.length()-1)ptr->frq=1;// first existance
    }
  }



  bool noChildren(const node*ptr){
    for(short i=0; i<alphaSize; i++)
      if(ptr->children[i])
        return 0;
    return 1;
  }


  void complete(node*ptr, string curr_word){
    if(ptr->frq){ // found a word
      item t(curr_word,ptr->frq); // new item to make it easy to push in the priorety queue
      words.push(t);
    }

    for(short i=0; i<alphaSize; i++){
      if(!ptr->children[i])continue;

      // backtracking to generate and search for all
      curr_word+=('a'+i);
      complete(ptr->children[i],curr_word);
      curr_word.pop_back();
    }
  }


  short predict(const string word){
    node*ptr=root;

    for(short i=0; i<word.length(); i++){
      short index=valOfchar(word[i]);
      if(ptr->children[index])
        ptr=ptr->children[index];
      else return -1; // it's not a saved word
    }
    if(noChildren(ptr))return 0;// there's no predictions

    complete(ptr,word);
    return 1;
  }


}pred;


bool isWantedToRemove(const char c){
    if(isalpha(c) or c==' ')return 0;
    return 1;
}



vector<string> getDataReady(){
    cout<<"     finishing buffer....\n";
    ifstream read("big.txt");// open the file and read it to put it into a string
    if(read.is_open()){
        stringstream out;
        out << read.rdbuf();
        string words = out.str();// we put it into string to split it later
        words.erase(remove_if(words.begin(),words.end(),isWantedToRemove),words.end());// remove all punc. marks

        cout<<"     finishing splitting....\n";
        istringstream iss(words);
        vector<string> tokens{istream_iterator<string>{iss},// make a vector and put tokens
                          istream_iterator<string>{}}; // of the string in it
        return tokens;
    }
    else{
        cout<<"can't open the database file!\n";
        return vector<string>(); // return an empty vector
    }
}


void addDataTotree(trie&pred, const vector<string>&words){
    for(long long i=0; i<words.size(); i++)
        pred.addNode(words[i]);// send every word to the tree
}

int main(){

    cout<<"1. getting data ready....\n";
    vector<string> words=getDataReady();
    cout<<"2. making the tree....\n";
    addDataTotree(pred,words);
    cout<<"\t\t\tfinished!\n\n";
    short res=pred.predict("foo");
    if(res==1)pred.display(5);
    else if(res==-1) cout<<"!!!!!!!!!! sorry, can't find this word !!!!!!!!!!!\n";
    else cout<<"!!!!!!!!!! sorry, there's no predictions !!!!!!!!!!!\n";

}
