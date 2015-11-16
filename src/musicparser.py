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
	measureAttributeTag = measure[0][1]
	time = measureAttributeTag.find('time')
	if time is not None:
		print "> time signature: " + "%s" % time[0].text + "/%s" % time[1].text

	#get tempo from the sound tag (inside a measure) if there is one
	if measure[0].find('sound') is not None:
		tempo = measure[0][2]
		if tempo.tag == 'sound':
			print "> tempo: %s" % tempo.attrib['tempo']

	#get dynamics tag (inside a measure) if there is one
	# if measure.find('part') is not None:
	for part in measure:
		for partElem in part:
			if partElem.find('direction') is not None:
				direction = partElem.find('direction')
				print direction.tag
		# if part.find('direction') is not None:
		# 	direction = part.iter('direction')
		# 	if direction.find('direction-type') is not None:
		# 		print "*****direction type ******"
		# 		directionType = direction.iter('direction-type')
		# 		if directionType.find('dynamics') is not None:
		# 			dynamics = directionType.find('dynamics')
		# 			print "****dynamics****"

print "\n"
print "Done."