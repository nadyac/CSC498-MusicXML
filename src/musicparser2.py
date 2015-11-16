#!/usr/bin/python

import xml.etree.ElementTree as ET

# Open XML document using minidom parser
DOMTree = ET.parse("timewise-ActorPrelude.xml")
#DOMTree = ET.parse("timewise-ActorPrelude.xml")
root = DOMTree.getroot()

print "Instrument parts and ID:"
print "========================"
for part in root.iter('score-part'):
	print "%s" % part[0].text + ", id: %s" % part.attrib['id']

print "\n"
print "Measures:"
print "==========="
for measure in root.iter('measure'):
	print "measure " + "%s" % measure.attrib['number']
	#attributes tag that contains the beats
	time = measure.find('time')
	if time is not None:
		print "> time signature: " + "%s" % time[0].text + "/%s" % time[1].text
	#get tempo from the sound tag
	part = measure.find('part')
	attributes = part.find('attributes')

	if attributes is not None:
		tempo = attributes.find('sound')
		print tempo
	else:
		tempo = "none."
	if tempo is not None and tempo is not 'none.' and tempo.tag == 'sound':
		print "> tempo: %s" % tempo.attrib['tempo']



print "\n"
print "Done."