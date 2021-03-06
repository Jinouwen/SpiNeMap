# PyCARL
Files to interface CARLsim to the pyNN framework.

# Command to run test.py with the CARLsim engine
```
  python <application>.py <pyNN_backend> <application_name> <log_mode> <ithGPUs> <randSeed>

$ python test.py carlsim "test" USER 1 42
```

# Carlsim Folder
1. /carlsim folder contains the python files required to interface pyNN to CARLsim5.
2. The contents of /carlsim are generated using the contents in the source folder. 
 

# Source Folder
1. Follow the steps below to compile a new pyNN interface (carlsim.i and carlsim_wrap.cxx) with the static library libcarsim.a (generated during a
   CARLsim build). (command also in the notes folder)


#####################################################################################  
# 1. Installation 

pyCARL has been tested on the following platforms:
1. Mac OSX 10.13
2. Ubuntu 16.04

Ubuntu Users: 
1. Users on an Ubuntu platform can install pyCARL in USER and DEV mode (explained below). 

Mac OSX Users: 
1. Users on a MacOSX platform will have to install pyCARL in DEV mode ONLY. (explained below)

There are two modes of installation: 
1. USER mode: Users that only want to use pyNN with CARLsim must install pyCARL 
using the USER installation steps. 
2. DEV mode: Users that want to develop custom applications for the pyCARL interface should
follow the Dev installtion steps. 

##################################################################################### 

# 1.1 Installation USER mode: 

##################################################################################### 

# 1.1.1 Installing PyNN:  
```
$ pip install pyNN 
```
Or follow the instructions in  
```
http://neuralensemble.org/docs/PyNN/installation.html 
```

###################################################################################### 

# 1.1.2 Install PyCARL  

1.1.1.1 Clone CARLsim5 
```
$ git clone <CARLsim5 github repo>
```

1.1.1.2 Move into the PyCARL directory in the CARLsim5 root
```
$ cd CARLsim5/pyCARL/ 
```

1.1.1.3 Copy the /carlsim folder to pyNN
```
$ cp -r CARLsim5/pyCARL/carlsim <pyNN installation location>
```

#####################################################################################  


# 1.2 Installation DEV mode: 

#####################################################################################

# 1.2.1 Installing PyNN:  

Ubuntu User:

```
$ pip install pyNN 
```
Or follow the instructions in  
```
http://neuralensemble.org/docs/PyNN/installation.html 
```
MacOSX user:

Install pyNN manually, using the sources in the pyNN github repo. 

Follow the steps in the link below to install pyNN manually:
```
http://neuralensemble.org/docs/PyNN/installation.html
```



# 1.2.2 Compile and Install pyCARL

1.1.1.1 Clone CARLsim5
```
$ git clone <CARLsim5 github repo>
```
1.1.1.2 Move into the PyCARL directory in the CARLsim5 root
```
$ cd CARLsim5/pyCARL
```


# 1.2.3 Compile and generate the pyNN -> carlsim  interface file (carlsim.py) 

# 1.2.3.1 Install SWIG:  
```
$ sudo apt update 

$ sudo apt install swig 
```
 

#1.2.3.2 Compile and link the interface file (carlsim_wrap.cxx), generated by swig, with the libcarlsim.a library.  
```
$ cd source  

Update the /include paths in the build.sh file (lines 8 or 12) 

Run the build.sh file. 

$ source build.sh 
```
This creates a static library _carlsim.so and a pyNN -> CARLsim interface (carlsim.py) 

 
# 3. Copy the compiled sources and the carlsim/ folder in the pyCARL repo to pyNN.  
```
$ cp ???r CARLsim5/pyCARL/carlsim <root of pyNN Installation>  
```
3.1 Copy the generated _carlsim.so and carlsim.py file to root-of-pyNN-Installation/carlsim 

 
PyCARL is now integrated with pyNN.  
 

