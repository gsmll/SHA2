## The magic school bus has forgotten their passwords to the school lab computers! **OH NO!**  


### Please use your hash cracking skills to retrieve their super-secret flags!  


##### Once you obtain 8 flags, submit them to google classroom

##### The server is @ 150.136.212.179 and the students are named

- James
- Logan
- Alex
- Noel
- Walter
- Isaac
- Drew
- Mike

##### All of their passwords follow [a-zA-Z]{3}[0-9]{2} format, according to class policy!

##### Ms. Frizzle has the hashes though! They are:

> c1a449398a972c4f6dc48ec057da8c48370817203a661771a81268c8fc28d01
> 57b3cb7c27965708c9abbe1c51a179495fd44ca61f132c2fc81b59b12ebcc8a2
> 8eb5848f901251cdcd16ae492db1e08b30a383b9fb8b4a26ae3c934e67f2f932329ab94336293f434c3aa36d2025ad42
> bb2e57dad2c7204ff0c433e2fe7ebfac584cdf1b872d2781d06f069066df87dd09e5030e6009b1358e896438e975a23adaaba767d2c520ff02a1364136044473
> b56726a196a0688213b9eae8dff46c4a372fd180826c6c84a291bdb8
> 1609ecdef30bd8b2a4270b30a28924f1518b576c2c77f122f7aa02f3
> 6a9abdf645861f8fc660b79c161bec23909f29428bdcd48a1ba22ee13560240069c88e861476d7cbcbd31692fd82966db3297c2e450273d938e55ba2d47a9f95  
> 18f3402bacf82fa962e888dbdb43131b324fb096eaa0a2974b7108565a6752421069db415beee8c559de2de77a12c083

##### They all use varying SHA-2 algos, find out which!

<details>
  <summary> Press For Help </summary>
  
  Use the sha shaker hash cracker, along with the list of hashes to crack the passwords of these 8 Accounts
  
  
  The Sha shaker has 3 flags, 
  - h for hash list ( can do multiple at once, seperated by newline)
  - w for word list 
  - f for form uses
  - - sha512
  - - sha256
  - - sha224
  - - sha384 
  
  to create the word list you can use this command:
  ```
  printf "%s\n" {\ ,{A..Z},{a..z}}{\ ,{A..Z},{a..z}}{\ ,{A..Z},{a..z}}{0..9}{0..9} > wordlist.txt
  ```
 
 
  
  </details>
  

