Description:

Note: This project is tested on Ubuntu Linux system !

 This program shall Monitor file creation in Test folder and once the file is created with specific name it will trigger:
  - Create disk usage information for all content of Target folder and store it in .\Target\CollectedData.txt
  - Archieve of Data in the Target folder
  - Store it in .\Storage folder
  
 --------------------------------------------
 ---------- Compilation
 --------------------------------------------
 To compile this code run the following command:
  - cmake CMakeLists.txt
  - make
  
 --------------------------------------------
 ---------- running
 --------------------------------------------  
  Preconditions:
  The following folders need to be created:
  	- Test: which is the folder to be monitored for file creation 
  	- Target: Is the folder that contain data to be archieved
  	- Storage: Is the folder where the archieve will be stored
  	
  To run the program call the following command:
   >> ./CollectionCmake Test Target Storage
   
   then start the creation of files with the specificed criteria like:
   "coreTest.yy.kk.12355FFFFABCDEFEFFABCFFEAA.lz4"
   
   then the archieve shall be created in the Storage folder
