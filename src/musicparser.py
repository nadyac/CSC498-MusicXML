#!/usr/bin/python

import xml.etree.ElementTree as ET
import io

# Open XML document using minidom parser
fileName = "timewise-ActorPrelude.xml"
DOMTree = ET.parse(fileName)
root = DOMTree.getroot()

outfile = fileName.replace(".",'')
with open(outfile + "-output.xml", mode='wt') as outputFile:

	print "Instrument parts and ID:"
	print "========================"
	for part in root.iter('score-part'):
		print "%s" % part[0].text + ", id: %s" % part.attrib['id']

	print "\n"
	print "Measures:"
	print "==========="
	for measure in root.iter('measure'):
		measure_str = "measure " + "%s" % measure.attrib['number']
		print measure_str
		outputFile.write(measure_str + "\n")

		# attributes tag that contains the beats
		measureAttributeTag = measure[0][1]
		time = measureAttributeTag.find('time')
		if time is not None:
			time_signature = "time signature: " + "%s" % time[0].text + "/%s" % time[1].text
			print "\t" + time_signature
			outputFile.write(time_signature + "\n")

		# get tempo from the sound tag (inside a measure) if there is one
		if measure[0].find('sound') is not None:
			tempo = measure[0][2]
			if tempo.tag == 'sound':
				tempo_str = "tempo: %s" % tempo.attrib['tempo']
				print "\t" + tempo_str
				outputFile.write(temp_str + "\n")
		else: 
			print "no tempo data."

		# get dynamics tag (inside a measure) if there is one
		# if measure.find('part') is not None:
		for part in measure: #part
			for partElem in part: #direction
				for subElems in partElem: #direction-type
					if subElems.find('dynamics') is not None:
						dynamicsTag = subElems.find('dynamics')
						dynamics_str = dynamicsTag[0].tag
						print "\t" + part.attrib['id'] + " dynamic-> " + dynamicsTag[0].tag
						outputFile.write(part.attrib['id'] + " " + dynamics_str + "\n")

outputFile.close()
print "\n"
print "Done."