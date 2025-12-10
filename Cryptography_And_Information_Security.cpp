#include <iostream>
#include <string>
#include <bitset>
using namespace std;

//to run
//g++ Cryptography_And_Information_Security.cpp -o crypto
//.\crypto
string* roundKeys;

void removeSpace(string msg,string* &arr,int& count){
	int idx=0;
	string modefied="",curr="";

	for(char c:msg)if(c!=' ')modefied+=c;
    count=modefied.size()/8;
    if(modefied.size()%8!=0)count++;

	arr=new string[count];
	for(int j=0;j<modefied.length();j++){
        curr+=modefied[j];
		if(curr.length()==8){
			arr[idx++]=curr;
			curr="";
		}
	}

	if(curr.length()>0)arr[idx]=curr;

}

void ConvertToBits(string arr[],int count,string* &bitArr){
	string Padding="TUVWXYZ";
	for(int k=arr[count-1].length();k<=8;k++)arr[count-1]+=Padding[k-1];
	bitArr=new string[count];
	for(int i=0;i<count;i++){
		string bits = "";
		for(char c:arr[i]) {
        	bits+=bitset<8>(c).to_string();
    	}
		bitArr[i]=bits;
	}
}

int PC1[56]={
    57,49,41,33,25,17,9,
    1,58,50,42,34,26,18,
    10,2,59,51,43,35,27,
    19,11,3,60,52,44,36,
    63,55,47,39,31,23,15,
    7,62,54,46,38,30,22,
    14,6,61,53,45,37,29,
    21,13,5,28,20,12,4
};

int PC2[48]={
    14,17,11,24,1,5,
    3,28,15,6,21,10,
    23,19,12,4,26,8,
    16,7,27,20,13,2,
    41,52,31,37,47,55,
    30,40,51,45,33,48,
    44,49,39,56,34,53,
    46,42,50,36,29,32
};

int SHIFTS[16]={1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};

string leftShift(string &s,int n){
    return s.substr(n)+s.substr(0,n);
}

void generateKeys(string key64){
    roundKeys=new string[16];

    string key56 = "";
    for (int i=0;i<56;i++)key56+=key64[PC1[i]-1];

    string C=key56.substr(0,28);
    string D=key56.substr(28,28);

    for (int r = 0; r < 16; r++) {
		C=leftShift(C,SHIFTS[r]);
        D=leftShift(D,SHIFTS[r]);
        string CD=C+D;

        string subkey="";
        for (int i=0;i<48;i++)subkey+=CD[PC2[i]-1];
        roundKeys[r]=subkey;
    }
}

void keyVerification(string& key){
	if(key.size()>8){
		string temp="";
		temp=key.substr(0, 8);
	}else if(key.size()<8){
		string Padding="TUVWXYZ";
		for(int k=key.size();k<=8;k++)key+=Padding[k-1];
	}
	string keyBits = "";
	for(char c:key) {
    	keyBits+=bitset<8>(c).to_string();
    }
	generateKeys(key);
}

string initialPermutation(const string &input64){
    int IP[64]={
        58,50,42,34,26,18,10,2,
        60,52,44,36,28,20,12,4,
        62,54,46,38,30,22,14,6,
        64,56,48,40,32,24,16,8,
        57,49,41,33,25,17,9,1,
        59,51,43,35,27,19,11,3,
        61,53,45,37,29,21,13,5,
        63,55,47,39,31,23,15,7
    };
    string output="";
    for(int i=0;i<64;i++)output+=input64[IP[i]-1];
    return output;
}

string finalPermutation(const string &input64){
    int INV_IP[64]={
        40,8,48,16,56,24,64,32,
        39,7,47,15,55,23,63,31,
        38,6,46,14,54,22,62,30,
        37,5,45,13,53,21,61,29,
        36,4,44,12,52,20,60,28,
        35,3,43,11,51,19,59,27,
        34,2,42,10,50,18,58,26,
        33,1,41,9,49,17,57,25
    };
    string output="";
    for(int i=0;i<64;i++)output+=input64[INV_IP[i]-1];
    return output;
}

void ExpansionBox(string& right){
    int idx=0,size=right.size();
    string currbits="";
    currbits+=right[size-1];
    while(idx<size){
        for(int i=0;i<4;i++){
            currbits+=right[idx];
            idx++;
        }
        if(idx<size-1){
            currbits+=right[idx];
            currbits+=right[idx-1];
        }else currbits+=right[0];
    }
    cout<<currbits;
}

string XOR(string& left,string& right){
    string xorBit="";
    for(int i=0;i<left.size();i++){
        if(left[i]==right[i])xorBit+="0";
        else xorBit+="1";
    }
    return xorBit;
}

string SBOX(string& bits){
    int i=0;
    string curr="";
    while(i<bits.size()){
        for(int j=i;j<i+6;j++)curr+=bits[j];
        string column="",row="";
        row+=curr[0];
        row+=curr[5];
        column=curr.substr(1,4);
        i+=6;
    }
    return "";
}

string permutation(string& bits){
    return "";
}

string function(string& right,int& roundNo){
    ExpansionBox(right);
    string output=XOR(right,roundKeys[roundNo]);
    string output2=SBOX(output);
    return permutation(output2);
}

string performRounds(string& currBits){
	string left=currBits.substr(0,32);
    string right=currBits.substr(32,32);
	int roundNo=0;
	for(int i=0;i<8;i++){
		string output=function(right,roundNo);
		string output2=XOR(left,output);
		roundNo++;
		string output3=function(output2,roundNo);
		string right=XOR(right,output3);
		left=output2;
	}
	return left+right;
}

string Swapping(){
    return "";
}

void performDES(string& currBits){
	string step1=initialPermutation(currBits);
	string step2=performRounds(step1);
	string step3=Swapping();
	string step4=finalPermutation(step3);
}

void DES(string& msg,string& key){
	int count=0;
	string* arr;
	string* bitArr;

	keyVerification(key);

	removeSpace(msg,arr,count);

    ConvertToBits(arr,count,bitArr);

	for(int i=0;i<count;i++){
		performDES(arr[count]);
	}
}

int main(){
	// string msg="",key="",Ent;
	// cout<<"Enter the Massage: ";
    // getline(cin, msg);
	// cout<<"Enter the Key: ";
	// cin>>key;

	// DES(msg,key);
    string str="123456";
    SBOX(str);
}
// before the Inverse permutation is there any swapping??