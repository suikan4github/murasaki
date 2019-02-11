# HOW TO GENERATE Murasaki Document

To generate the doxygen documents, there are two way.

- Generate the document from SW4STM32
- Generate the document from Ubuntu terminal window

## Generate the document from SW4STM32

Be sure the SW4STM32 is installed, and the murasaki project ( this
directory ) is imported to the workspace.

 1. From the menu bar, choose Help->Eclipse Market place...
 1. From the Find input control, type "eclox", then type return.
 1. Install the eclox, including doxygen binary.
 1. Choose the murasaki project and right click.
 1. From the context menu, do the Build Documentation.

You will see the documents in the html sub-directory.

## Generate the document from Ubuntu terminal window

Followings are tested in the Ubuntu 16.04 LTS

1. Open the terminal window
1. execute following :
```
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install doxygen
cd murasaki # this directory
doxygen murasaki.doxyfile
```
 You will see the documents in the html sub-direcotry.

## How to make PDF manual

To make a PDF manual, follow :

1. Open the terminal window
2. execute following
```
 sudo apt-get install texlive-latex-base texlive-fonts-recommended texlive-fonts-extra texlive-latex-extra
 cd murasaki # this directory
 cd latex
 make
```
The file name is refman.pdf.

## Where to read first

Open the html file in the html directory, then, click Related Pages
-> A user's guide of Murasaki Class library

Or open murasaki_ug.html file.
