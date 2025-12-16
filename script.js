let Mode=document.querySelector(".Mode")
const encryption=document.querySelector(".EncBtn")
const decryption=document.querySelector(".DecBtn")
const part1=document.querySelector(".part1")
const part2=document.querySelector(".part2")
const uploadbox2=document.querySelector('.upload-box2')
const heading=document.querySelector('.heading')

decryption.addEventListener("click",()=>{
    Mode.checked=false;
    part1.style.display="none"
    part2.style.display="block"
    heading.innerHTML="<h2>DES Decryption</h2>"
})

encryption.addEventListener("click",()=>{
    Mode.checked=true;
    part1.style.display="block"
    part2.style.display="none"
    heading.innerHTML="<h2>DES Encryption</h2>"
})

class DESCryptoService{

    stringToBinary(str){
        return str.split("")
            .map(c=>c.charCodeAt(0).toString(2).padStart(8,"0"))
            .join("");
    }

    binaryToString(bin){
        let result="";
        for(let i=0;i<bin.length;i+=8){
            result+=String.fromCharCode(parseInt(bin.substr(i,8),2));
        }
        return result.replace(/\0/g,"");
    }

    xor(a,b){
        return a.split("").map((bit,i)=>bit ^ b[i]).join("");
    }

    generateKeys(key){
        let keys=[];
        let binKey=this.stringToBinary(key).padEnd(64,"0").slice(0,64);

        for(let i=0;i<16;i++){
            binKey=binKey.slice(1) + binKey[0];
            keys.push(binKey.slice(0,32));
        }
        return keys;
    }

    feistel(right,key){
        return this.xor(right,key);
    }

    encrypt(plainText,key){
        let bin=this.stringToBinary(plainText).padEnd(64,"0").slice(0,64);
        let left=bin.slice(0,32);
        let right=bin.slice(32);

        const keys=this.generateKeys(key);

        for(let i=0;i<16;i++){
            let temp=right;
            right=this.xor(left,this.feistel(right,keys[i]));
            left=temp;
        }

        return left + right;
    }

    decrypt(cipherBin,key){
        let left=cipherBin.slice(0,32);
        let right=cipherBin.slice(32);

        const keys=this.generateKeys(key).reverse();

        for(let i=0;i<16;i++){
            let temp=left;
            left=this.xor(right,this.feistel(left,keys[i]));
            right=temp;
        }

        return this.binaryToString(left + right);
    }
}


class SteganographyService{
    static DELIMITER="1111111111111110";

    static hideMessageInImage(img,hexMessage,callback){
        const canvas=document.createElement("canvas");
        const ctx=canvas.getContext("2d");

        canvas.width=img.width;
        canvas.height=img.height;
        ctx.drawImage(img,0,0);

        let imageData=ctx.getImageData(0,0,canvas.width,canvas.height);
        let data=imageData.data;

        let binaryMsg="";
        for(let i=0;i<hexMessage.length;i+=2){
            const hexByte=hexMessage.substr(i,2);
            const byte=parseInt(hexByte,16);
            binaryMsg+=byte.toString(2).padStart(8,'0');
        }
        
        binaryMsg+=this.DELIMITER;
        let msgIndex=0;

        for(let i=0;i<data.length && msgIndex<binaryMsg.length;i+=4){
            data[i]=(data[i] & 254) | parseInt(binaryMsg[msgIndex],10);
            msgIndex++;
        }

        if(msgIndex<binaryMsg.length){
            console.warn('Message too long for image,truncated');
        }

        ctx.putImageData(imageData,0,0);
        callback(canvas.toDataURL());
    }

    static extractMessageFromImage(img){
        const canvas=document.createElement("canvas");
        const ctx=canvas.getContext("2d");

        canvas.width=img.width;
        canvas.height=img.height;
        ctx.drawImage(img,0,0);

        let data=ctx.getImageData(0,0,canvas.width,canvas.height).data;
        let bits="";

        for(let i=0;i<data.length;i+=4){
            bits+=(data[i] & 1);
            if(bits.endsWith(this.DELIMITER)) break;
        }

        bits=bits.replace(this.DELIMITER,"");
        
        let hexMessage="";
        for(let i=0;i<bits.length;i+=8){
            const byte=bits.substr(i,8);
            if(byte.length===8){
                const decimal=parseInt(byte,2);
                hexMessage+=decimal.toString(16).padStart(2,'0');
            }
        }
        
        return hexMessage;
    }
}


class EncryptionApp{
    constructor(){
        this.selectedImage=null;
        this.desCrypto=new DESCryptoService();
        this.init();
    }

    init(){
        this.bindElements();
        this.setupEventListeners();
        this.setupInitialState();
        
        this.resultText=document.querySelector(".Result p");
        this.resultImage=document.querySelector(".Result img");
        this.decryptResult=document.querySelector(".result2");
        
        if(this.resultText){
            this.resultText.innerText="Encrypted result will appear here...";
            this.resultText.style.color="#666";
        }
        
        if(this.decryptResult){
            this.decryptResult.innerText="Decrypted text will appear here...";
            this.decryptResult.style.color="#666";
        }
    }
    
    bindElements(){
        this.images=document.querySelector(".Images");
        this.img1=document.querySelector('.image1');
        this.img2=document.querySelector('.image2');
        this.img3=document.querySelector('.image3');
        this.uploadbox=document.querySelector('.upload-box');
        this.uploadTxt=document.querySelector('.image p:nth-of-type(2)');// Second p tag
        this.uploadInput=document.getElementById("imageUpload");
        this.uploadInput2=document.getElementById("imageUploadDec");
        this.encryptionBtn=document.querySelector(".EncBtn");
        this.decryptionBtn=document.querySelector(".DecBtn");
        this.modeCheckbox=document.querySelector(".Mode");
        this.part1=document.querySelector(".part1");
        this.part2=document.querySelector(".part2");
        this.heading=document.querySelector('.heading');
        this.encryptBtn=document.querySelector(".GenerateBtn button");
        this.decryptBtn=document.querySelector(".GenerateBtn2 button");
        this.resultImage=document.querySelector(".Result img");
        this.resultText=document.querySelector(".Result p");
        this.decryptResult=document.querySelector(".result2");
        this.afterUploading=document.querySelector(".after-uploading")
        this.afterUploading2=document.querySelector(".after-uploading2")
    }

    setupEventListeners(){
        this.images.addEventListener("change",()=>this.handleImageChange());
        this.uploadInput.addEventListener("change",(e)=>this.handleEncryptionUpload(e));
        this.uploadInput2.addEventListener("change",(e)=>this.handleDecryptionUpload(e));
        this.encryptionBtn.addEventListener("click",()=>this.switchToEncryption());
        this.decryptionBtn.addEventListener("click",()=>this.switchToDecryption());
        this.encryptBtn.addEventListener("click",()=>this.performEncryption());
        this.decryptBtn.addEventListener("click",()=>this.performDecryption());
    }

    setupInitialState(){
        this.hideAll();
        this.img1.style.display="block";
        this.selectedImage=this.img1;
    }

    hideAll(){
        this.img1.style.display="none";
        this.img2.style.display="none";
        this.img3.style.display="none";
        this.uploadbox.style.display="none";
        
        const preview=document.querySelector(".uploaded-preview");
        if(preview){
            preview.style.display="none";
        }
    }

    handleImageChange(){
        const selected=this.images.value;
        
        if(selected==="image1"){
            this.hideAll();
            this.img1.style.display="block";
            this.selectedImage=this.img1;
        } else if(selected==="image2"){
            this.hideAll();
            this.img2.style.display="block";
            this.selectedImage=this.img2;
        } else if(selected==="image3"){
            this.hideAll();
            this.img3.style.display="block";
            this.selectedImage=this.img3;
        } else if(selected==="upload-box"){
            this.hideAll();
            this.uploadbox.style.display="flex";
            const preview=document.querySelector(".uploaded-preview");
            if(preview){
                preview.style.display="block";
                const tempImg=new Image();
                tempImg.src=preview.src;
                tempImg.onload=() =>{
                    this.selectedImage=tempImg;
                };
            } else{
                this.selectedImage=this.img1;
            }
        }
    }

    handleEncryptionUpload(event){
        const file=event.target.files[0];
        if(!file) return;

        if(!file.type.startsWith("image/")){
            this.afterUploading.style.color="red";
            this.afterUploading.innerText="Please upload an image file!";
            return;
        }

        const reader=new FileReader();
        reader.onload=(e) =>{
            const img=new Image();
            img.onload=() =>{
                this.selectedImage=img;

                const old=document.querySelector(".uploaded-preview");
                if(old) old.remove();

                const preview=document.createElement("img");
                preview.src=e.target.result;
                preview.className="uploaded-preview";

                document.querySelector(".image").appendChild(preview);

                this.afterUploading.style.color="green";
                this.afterUploading.innerText=`✓ ${file.name} uploaded`;

                this.images.value="upload-box";
                this.hideAll();
                this.uploadbox.style.display="flex";
            };
            img.src=e.target.result;
        };
        reader.readAsDataURL(file);
    }

    handleDecryptionUpload(event){
        const file=event.target.files[0];
        if(!file) return;

        if(!file.type.startsWith("image/")){
            this.afterUploading2.style.color="red";
            this.afterUploading2.innerText="Please upload an image file!";
            return;
        }

        this.afterUploading2.style.color="green";
        this.afterUploading2.innerText=`✓ ${file.name} ready for decryption`;
    }

    switchToEncryption(){
        this.modeCheckbox.checked=true;
        this.part1.style.display="block";
        this.part2.style.display="none";
        this.heading.innerHTML="<h2>DES Encryption</h2>";
    }

    switchToDecryption(){
        this.modeCheckbox.checked=false;
        this.part1.style.display="none";
        this.part2.style.display="block";
        this.heading.innerHTML="<h2>DES Decryption</h2>";
    }

    async performEncryption(){
        const text=document.querySelector(".PlainTxt input").value;
        const key=document.querySelector(".KeyIp input").value;

        if(!text || !key){
            alert("Plain text and key are required!");
            return;
        }

        if(!this.selectedImage){
            alert("Please select an image!");
            return;
        }

        if(key.length<8){
            alert("Key must be at least 8 characters long!");
            return;
        }

        try{
            this.resultText.innerText="Encrypting and hiding text in image...";
            this.resultText.style.color="#3f51b5";
            
            this.resultImage.src="";
            this.resultImage.style.display="none";
            
            const oldLink=document.querySelector('.download-link');
            if(oldLink) oldLink.remove();

            const cipherBin=this.desCrypto.encrypt(text,key);
            
            let cipherHex="";
            for(let i=0;i<cipherBin.length;i+=4){
                const bits=cipherBin.substr(i,4);
                cipherHex+=parseInt(bits,2).toString(16);
            }
            
            SteganographyService.hideMessageInImage(this.selectedImage,cipherHex,(encodedImg) =>{
                this.resultImage.src=encodedImg;
                this.resultImage.style.display="block";
                this.resultImage.style.maxWidth="100%";
                this.resultImage.style.maxHeight="200px";
                this.resultImage.style.marginTop="10px";
                this.resultImage.style.borderRadius="8px";
                this.resultImage.style.border="1px solid #ddd";
                
                this.resultText.innerText="✓ Encryption successful! Text hidden in image.";
                this.resultText.style.color="#4CAF50";
                
            });
            
        } catch(error){
            console.error("Encryption error:",error);
            this.resultText.style.color="#f44336";
            this.resultText.innerText=`✗ Encryption failed: ${error.message}`;
        }
    }

    async performDecryption(){
        const key=document.querySelector(".KeyIp2 input").value;
        const fileInput=document.getElementById("imageUploadDec");

        if(!fileInput.files[0] || !key){
            alert("Upload image & key!");
            return;
        }

        if(key.length<8){
            alert("Key must be at least 8 characters long!");
            return;
        }

        try{
            this.decryptResult.innerText="Decrypting...";
            this.decryptResult.style.color="#3f51b5";
            
            const reader=new FileReader();
            reader.onload=(e) =>{
                const img=new Image();
                img.onload=() =>{
                    try{
                        const cipherHex=SteganographyService.extractMessageFromImage(img);
                        
                        if(!cipherHex || cipherHex.length===0){
                            throw new Error("No encrypted data found in image");
                        }
                        
                        let cipherBin="";
                        for(let i=0;i<cipherHex.length;i+=2){
                            const hexByte=cipherHex.substr(i,2);
                            const decimal=parseInt(hexByte,16);
                            cipherBin+=decimal.toString(2).padStart(8,'0');
                        }
                        
                        const plainText=this.desCrypto.decrypt(cipherBin,key);
                        
                        this.decryptResult.innerHTML=`
                            <strong style="color: #333;">Decrypted Text:</strong><br>
                            <div style="background: white;padding: 15px;margin-top: 10px;
                                       border-radius: 8px;border: 1px solid #ddd;
                                       font-family: monospace;word-wrap: break-word;">
                                ${plainText}
                            </div>
                        `;
                        
                    } catch(error){
                        this.decryptResult.innerHTML=`
                            <span style="color: #f44336;">
                                ✗ Decryption failed: ${error.message}
                            </span>`;
                    }
                };
                
                img.src=e.target.result;
            };
            
            reader.readAsDataURL(fileInput.files[0]);
            
        } catch(error){
            this.decryptResult.innerHTML=`
                <span style="color: #f44336;">
                    ✗ Error: ${error.message}
                </span>`;
        }
    }
}


document.addEventListener('DOMContentLoaded',() =>{
    const app=new EncryptionApp();
    const style=document.createElement('style');
    style.textContent=`
        .uploaded-preview{
            display: block;
            max-width: 100%;
            max-height: 200px;
            margin-top: 15px;
            border-radius: 10px;
            border: 2px solid #3f51b5;
            box-shadow: 0 3px 10px rgba(0,0,0,0.1);
        }
        
        .download-link:hover{
            background-color: #45a049 !important;
        }
        
        .Result img{
            margin-top: 10px;
            border: 1px solid #ddd;
            border-radius: 5px;
            max-width: 100%;
            max-height: 200px;
        }
        
        .result2{
            padding: 15px;
            background-color: #f8f9fa;
            border-radius: 8px;
            border: 1px solid #e0e0e0;
            margin-top: 10px;
            word-wrap: break-word;
            min-height: 60px;
        }
        
        .upload-status{
            margin-top: 10px;
            font-size: 14px;
            min-height: 20px;
            color: #666;
        }
    `;
    document.head.appendChild(style);
});