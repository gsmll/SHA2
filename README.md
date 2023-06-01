<p align="center">
<font size="5">
  <h3 align="center" >Sha Sha Slide</h3>
</font>  
  <p align="center">
    Sha-2 Encryption in a Nutshell
    <br> </br>
    <a href="https://tryhackme.com/p/Jeffrey.J">
    <img src="https://i.imgur.com/dODfC6z.gif"  alt="GSmall" width=72 height=72>
    </a>
    <a href="https://tryhackme.com/p/GSmall">
    <img src="https://tryhackme-images.s3.amazonaws.com/user-avatars/c31ea6bfbcc1fa28101976dced850e48.png"  alt="GSmall" width=72 height=72>
    <br> 
    </a>
      <a href="https://tryhackme.com/p/Jeffrey.J">
        Jeffrey Jiang </a>
        <a href="https://tryhackme.com/p/GSmall">
          | Gabriel Small</a>
    </a>
  </p>
</p>  





# Project Description



#### This project covers string hashing and encryption using the SHA-2 algorithm and its subdivisions.  

<br> 

#### We recreated the SHA 224, 256, 384, and 512 algorithms in C++, along with a hashcat-like hash cracker to test these algorithms against a word list, such as rockyou. 


<br><br>
</br>

# Access and Usage



#### To use this project first clone the repo @
```
git clone git@github.com:Stuycs-K/final-project-4-jiangj-smallg.git
```
#### and run the make file, which should in turn create the Shaker binary, which will be in the tools directory. This created binary should be all you need for the homework associated with this project. 
<br>

#### **Mac Users, please use lab machines (more optimized). If really against it, read on**
<br>

#### if on mac, make sure to use g++ and not clang, as we use g++ specific instructions. Once installing gcc or g++ on your machine, using preferred methods such as homebrew, either change the makefile to use **g++-13** instad of **g++** or use the command:
<br>

```
sudo ln -s $(which g++-13) /usr/local/bin/g++
```


#### to overwrite the default which aliases g++ to clang.

#### to install g++ you can do
``` 
brew install gcc
```
<br></br><br>

# Links

### [Homework](HOMEWORK.md)
### [Presentation](PRESENTATION.md)



