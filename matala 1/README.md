# Operating_Systems Ex1
## Talya Shitreet 314855099 
## Renana Rimon 207616830

How to run from the command line?</br>
USE: 
  * make - to compile the program
  * make clean - to remove all compile files.
  * where wroten {file1} put the src file name, and {file2} is the dst file name. </br> 

1. **compare** 2 files content: </br>
   USE: cmp {file1} {file2} </br>
   ![image](https://user-images.githubusercontent.com/77155986/202903801-0e8c02b0-acc3-471c-906e-d3b98778c1b0.png)
   
2+3. **copy** file1 to file2:
  * copy file1 content to file2</br>
       USE: copy {file1} {file2} </br>
       ![image](https://user-images.githubusercontent.com/77155986/202903861-9008d818-fc88-49dd-8510-a68f7f25c76b.png)

  * **copying of symbolic links** in copy: </br>
    a. copy the symbolic link </br>
       USE: copy -l {file1} {file2} </br>
       ![image](https://user-images.githubusercontent.com/77155986/202903916-4e3e14cb-bca4-450b-823c-9cb62f9d4c96.png)

    b. copy the content of the file that the symbolic link is pointing to </br>
       USE: copy {file1} {file2} </br>
       ![image](https://user-images.githubusercontent.com/77155986/202903959-8c2123e2-1f39-477b-b72d-ee49f75f5098.png)


4. **Encode & Decode**: </br>
2 libraries: codec1.so, codec2.so, each library has 2 functions - encode and decode.</br>
  * codec1.so:</br>
    a. encode - converts uppercase chars to lowercase</br>
      USE: ./encode codec1 "your string"</br>
      ![image](https://user-images.githubusercontent.com/77155986/202904404-c08217e6-c6cc-444b-9862-f20a2cb45946.png)

    b. decode - converts lowercase chars to uppercase</br>
      USE: ./decode codec1 "encoded string" </br>
      ![image](https://user-images.githubusercontent.com/77155986/202904445-322f5ed9-265d-4327-9a08-a5d6f2a0d046.png)

  * codec2.so:</br>
    a. encode - calculates NEG value of each char</br>
      USE: ./encode codec2 "your string"</br>
      ![image](https://user-images.githubusercontent.com/77155986/202904507-529d1026-973e-49e8-a8ae-3abb52fb1b7b.png)

    b. decode - return back the encode string.</br>
      USE: ./decode codec2 "encoded string"</br>
      (to get a readble string, put in the encoded string!) </br>
      ![image](https://user-images.githubusercontent.com/77155986/202904611-6631d9b6-647f-4e14-b0eb-6fb5f400d8e2.png)
      
### ENJOY!

 internet code that we have used in our exercise:
 1. https://www.geeksforgeeks.org/add-1-to-a-given-number/
 2. https://www.geeksforgeeks.org/subtract-1-without-arithmetic-operators/

