# CSC498-MusicXML Parser
Music XML CSC498 research project. Accurate as of 3/01/2015.

## Project Description
The goal is to parse and extract sheet music features like temp, dynamic, and articulation and later map those features to animations that will help a conducting robot do their corresponding arm movements.

## Source Files Description
- musicParser.py - Most up-to-date MusicXML sheet music parser
- timewise-binchois.xml - sheet music in timewise musicXML format
- timewise-ActorPreludeSample.xml - sheet music for ActorPrelude in timewise musicXML format
## Project Libraries and Requirements
- python 2.7
- ElementTree lib for XML parsing
- MusicXML samples

## To run the Python Script that extracts MusicXML features
The following instructions assume that Python 2.7 is installed.

1) Using the terminal, navigate to the src directory containing musicParser.py and the XML sheet music.

2) Type py musicParser.py 

3) Results should print out to console. 

## To run the Processing UI with multiple windows
0) Install the Interfascia and G4P Processing. 

 <b>To install Interfacia and G4P:</b>
- At the top bar, click on the 'Sketch' tab
- Select the 'Import Library' option
- Type 'Interfacia' into the search box and it should come up
- Click 'install' to install Interfacia.
- Repeat the last two steps to install G4P.

## To convert from partwise to timewise musicXML format follow these steps:
0) Install the XSLT tool (MSXSL) for converting XML. 

To run the XSLT (MSXSL):
> msxsl.exe [MUSICXML FILE] [XSL STYLESHEET] -o [output_file_name.xml]

example:
msxsl.exe ActorPreludeSheetMusic.xml parttime.xsl -o output-sheet.xml
