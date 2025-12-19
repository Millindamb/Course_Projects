#include <iostream>
#include <string>
#include <bitset>
using namespace std;

//to run
//g++ Cryptography_And_Information_Security.cpp -o crypto
//.\crypto
string* roundKeys;
int pad,*spaces,s;

void removeSpace(string msg,string* &arr,int& count){
	int idx=0;
	string modefied="",curr="";

	for(char c:msg){
        if(c!=' ')modefied+=c;
        else s++;
    }

    int j=0;
    spaces=new int[s];
    for(int i=0;i<msg.size();i++){
        if(msg[i]==' '){
            spaces[j]=i;
            j++;
        }
    }
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

void ConvertToBits(string arr[],int& count,string* &bitArr){
	string Padding="TUVWXYZ";
    pad=8-arr[count-1].size();
    int L=arr[count-1].length();
    for(int k=L;k<8;k++)arr[count-1]+=Padding[k-L];
	bitArr=new string[count];
	for(int i=0;i<count;i++){
		string bits="";
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

    for(int r=0;r<16;r++){
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
		key=key.substr(0,8);
	}else if(key.size()<8){
		string Padding="TUVWXYZ";
		int L=key.size();
        for(int k=L;k<8;k++){
            key+=Padding[k-L];
        }
	}
	string keyBits="";
	for(char c:key) {
    	keyBits+=bitset<8>(c).to_string();
    }
	generateKeys(keyBits);
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

string ExpansionBox(string& right){
    int E[48]={
        32,1,2,3,4,5,
        4,5,6,7,8,9,
        8,9,10,11,12,13,
        12,13,14,15,16,17,
        16,17,18,19,20,21,
        20,21,22,23,24,25,
        24,25,26,27,28,29,
        28,29,30,31,32,1
    };

    string currbits="";
    for(int i=0;i<48;i++)currbits+=right[E[i]-1];
    return currbits;
}

string XOR(const string& left,const string& right){
    string xorBit="";
    for(int i=0;i<left.size();i++){
        if(left[i]==right[i])xorBit+="0";
        else xorBit+="1";
    }
    return xorBit;
}

int S_BOX[8][4][16]={
    {
        {14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
        {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
        {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
        {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}
    },
    {
        {15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
        {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
        {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
        {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}
    },
    {
        {10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
        {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
        {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
        {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}
    },
    {
        {7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
        {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
        {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
        {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}
    },
    {
        {2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
        {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
        {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
        {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}
    },
    {
        {12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
        {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
        {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
        {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}
    },
    {
        {4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
        {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
        {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
        {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}
    },
    {
        {13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
        {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
        {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
        {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}
    }
};

string SBOX(string& bits){
    int i=0;
    int box_num=0;
    string result="";
    while(i<bits.size()){
        string curr=bits.substr(i,6);
        string column="",row="";
        row+=curr[0];
        row+=curr[5];
        column=curr.substr(1,4);
        curr="";
        int r=bitset<2>(row).to_ulong(),c=bitset<4>(column).to_ulong();
        int val=S_BOX[box_num][r][c];
        result+=bitset<4>(val).to_string();
        box_num++;
        i+=6;
    }
    return result;
}

string permutation(string& bits){
    int P[32] = {
        16,7,20,21,
        29,12,28,17,
        1,15,23,26,
        5,18,31,10,
        2,8,24,14,
        32,27,3,9,
        19,13,30,6,
        22,11,4,25
    };
    string result="";
    for(int i=0;i<32;i++){
        result+=bits[P[i]-1];
    }
    return result;
}

string function(string& right,int& roundNo){
    string Expended=ExpansionBox(right);
    string output=XOR(Expended,roundKeys[roundNo]);
    string output2=SBOX(output);
    return permutation(output2);
}

string performRounds(string& currBits){
	string left=currBits.substr(0,32);
    string right=currBits.substr(32,32);
	int roundNo=0;
	for(int i=0;i<16;i++){
		string output=function(right,roundNo);
		string output2=XOR(left,output);
        if(i!=15){
            left=right;
            right=output2;
        }else{
            left=left;
            right=output2;
        }
		roundNo++;
	}
	return right+left;
}


string performDES(string& currBits){
	string step1=initialPermutation(currBits);
	string step2=performRounds(step1);
	string step3=finalPermutation(step2);
    return step3;
}

string ConvertToCipherText(string& bits,string& result,string& result2){
    string curr="";
    for(int i=0;i<64;i+=8){
        curr=bits.substr(i,8);
        char ch=(char)bitset<8>(curr).to_ulong(); 
        result+=ch;

        int val=bitset<8>(curr).to_ulong();
        char hex[3];
        sprintf(hex,"%02X",val);
        result2+=hex;
    }
    return result;
}

void DES(int& count,string* &bitArr,string& msg,string& key,string& result,string& result2,string* &cipherBits){
	string* arr;
    
	keyVerification(key);

	removeSpace(msg,arr,count);

    ConvertToBits(arr,count,bitArr);

    cipherBits=new string[count];
    string ciphertext[count];

	for(int i=0;i<count;i++){
		cipherBits[i]=performDES(bitArr[i]);
        ciphertext[i]=ConvertToCipherText(cipherBits[i],result,result2);
	}
}

string performRounds_Decryption(string& currBits){
    string left=currBits.substr(0,32);
    string right=currBits.substr(32,32);

    for(int i=15;i>=0;i--){
        string temp=left;
        string f_output=function(left,i);
        left=XOR(right,f_output);
        right=temp;
    }
    return left+right;
}

string performDES_Decryption(string& cipherBits){
    if(cipherBits=="")return "";
    string step1=initialPermutation(cipherBits);
    string step2=performRounds_Decryption(step1);
    string step3=finalPermutation(step2);
    return step3;
}

string ConvertBitsToText(string& bits){
    string result="";
    for(int i=0;i<64;i+=8){
        string byte=bits.substr(i,8);
        char ch=(char)bitset<8>(byte).to_ulong();
        result+=ch;
    }
    return result;
}


string DES_decryption(string& cipherText,string* &bitArr,string* &cipherbits,int count){
    string curr="",result,Cipherbits="";
    string str=performDES_Decryption(Cipherbits);
    for(int i=0;i<count;i++)curr+=cipherbits[i];
    for(int i=0;i<count;i++){
        string currStr="";
        for(int j=0;j<64;j+=8){
            string currBits=bitArr[i].substr(j,8);
            char ch=(char)bitset<8>(currBits).to_ulong();
            currStr+=ch;
        }
        result+=currStr;
    }
    int len=result.size();
    result=result.substr(0,len-pad);
    for(int i=0;i<s;i++)result.insert(spaces[i]," ");
    return result;
}

int main(){
    int count=0;
	string msg="",key="",Ent,result,result2,*cipherBits,*bitArr;
    cout<<"===== DES (Data Encryption Standard) =====\n";
	cout<<"Enter the Massage Below.\n>";
    getline(cin,msg);
	cout<<"Enter the Key Below.\n>";
	cin>>key;
    cout<<"=================================\n\n";

	DES(count,bitArr,msg,key,result,result2,cipherBits);
    cout<<"===== DES Encryption Output =====\n";
    cout<<"Cipher Text (Raw Characters):\n";
    cout<<result<<endl<<endl;

    cout<<"Cipher Text (Hexadecimal Representation):\n";
    cout<<result2<<endl<<endl;

    cout<<"Key Used for Encryption : ";
    cout<<key<<endl;
    cout<<"=================================\n\n";

    cout<<"===== DES Encryption Output =====\n";
    cout<<DES_decryption(result,bitArr,cipherBits,count)<<endl;
    cout<<"=================================\n";
}