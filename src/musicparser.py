#!/usr/bin/python

from xml.dom.minidom import parse
import xml.dom.minidom

# Open XML document using minidom parser
DOMTree = xml.dom.minidom.parse("ActorPreludeSample.xml")
# collection = DOMTree.documentElement
# if collection.hasAttribute("score-partwise"):
#    print "Root element : %s" % collection.getAttribute("score-partwise")

collection = DOMTree.documentElement
print collection
print collection.getElementsByTagName("movement-title")[0]

# Get all some of the music attributes in the collection
musicAttributes = DOMTree.getElementsByTagName("part-name")[0]
print musicAttributes.childNodes[0].data

attributes = DOMTree.getElementsByTagName("part-name")
# Print detail of each attribute chosen.
for attribute in attributes:
    print " "
    print "*****sheetmusic attributes*****"

    type = attribute.getElementsByTagName('score-part')
    print "Instrument: %s" % attribute.childNodes[0].data
    #print "Instrument: %s" % type