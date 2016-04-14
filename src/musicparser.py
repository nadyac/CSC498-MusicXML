#!/usr/bin/python

import xml.etree.ElementTree as ET
import io

# Open XML document using minidom parser
fileName = "timewise-ActorPrelude.xml"
DOMTree = ET.parse(fileName)
root = DOMTree.getroot()
partStatus = []
dynamic = ""

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
		measureNumber = "measure " + "%s" % measure.attrib['number']
		print measureNumber
		outputFile.write(measureNumber + ", ")

		# attributes tag that contains the beats
		measureAttributeTag = measure[0][1]
		time = measureAttributeTag.find('time')
		if time is not None:
			time_signature = "time signature: " + "%s" % time[0].text + "/%s" % time[1].text
			#print "\t" + time_signature
			outputFile.write(time_signature + ", ")

		# print measure[0]
		for direction in measure.iter('direction'):
			if direction.find('direction-type') is None:
				continue
			else:
				directionType = direction.findall('direction-type')
				for m in directionType: 
					metronome = m.find('metronome')
					if metronome is not None:
						for tags in metronome:
							tempo = tags.text
							print "\t" + tags.text
							outputFile.write(tempo + ", ")

		# get dynamics tag (inside a measure) if there is one
		# if measure.find('part') is not None:
		for part in measure: #part
			for partElem in part: #direction
				for subElems in partElem: #direction-type
					if subElems.find('dynamics') is not None:
						dynamicsTag = subElems.find('dynamics')
						dynamic = dynamicsTag[0].tag
						#print "\t" + part.attrib['id'] + " dynamic-> " + dynamic
						outputFile.write(part.attrib['id'] + " " + dynamic + ", ")
			# find all notes played by this part in current measure
			notes = part.findall('note')
			for note in notes: # look at tags inside each individual note for current part
				for noteTags in note:
				 if note.find('rest') is not None:
				 	restTag = note.find('rest')
				 	if restTag.get('measure') is not None:
				 		rest = restTag.get('measure')
						partStatus.append(dynamic)
				 	else:
				 		partStatus.append("REST")
						 	#partStatus.append(part.attrib['id'])
		print partStatus
		partStatus = []


outputFile.close()
print "\n"
print "Done."