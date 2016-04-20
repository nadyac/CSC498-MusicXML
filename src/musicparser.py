#!/usr/bin/python

import xml.etree.ElementTree as ET
import io

def compareLists(listA, listB):
	cueParts = []
	print "Parts to Cue:"
	for itemA in listA:
		for itemB in listB:
			if itemA is "REST" and itemB is not "REST":
				cueParts.append(itemB)
	return cueParts


def main():
	# Open XML document using minidom parser
	fileName = "timewise-ActorPrelude.xml"
	DOMTree = ET.parse(fileName)
	root = DOMTree.getroot()
	partStatus = []
	partStatusArray = [] #should have 2 partStatus arrays in it
	partStatusArrayNum = 0
	partsToCue = []
	dynamic = ""
	tempo = ""

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
			outputFile.write("\n")
			outputFile.write(measureNumber + ", ")

			# attributes tag that contains the time signature (beats)
			measureAttributeTag = measure[0][1]
			time = measureAttributeTag.find('time')
			if time is not None:
				timeSignature = "time signature: " + "%s" % time[0].text + "/%s" % time[1].text
				print timeSignature
			else: 
				timeSignature = "time signature: none"

			outputFile.write(timeSignature + ", ")

			# print measure[0] line 528
			for direction in measure.iter('direction'):
				if direction.find('sound') is not None:
					soundTag = direction.find('sound')
					if soundTag.get('tempo') is not None:
						tempo = soundTag.get('tempo')
						print "tempo: " + tempo
						break
			# 	else:
			# 	 	directionType = direction.findall('direction-type')
			# 	 	for m in directionType: 
			# 	 		metronome = m.find('metronome')
			# 	 		if metronome is not None:
			# 	 			for tags in metronome:
			# 	 				tempo = "tempo: " + tags.text
			# 	 				continue
			# 	 		else:
			# 	 			tempo = "tempo: null"

			outputFile.write("tempo: " + tempo + ", ")

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
				note = part.find('note')
				if note.find('rest') is not None:
					partStatus.append("REST")
				else:
					partStatus.append(part.attrib['id'])

			# for every measure get partStatus
			#print partStatus
			outputFile.write("[")

			for item in partStatus:
				outputFile.write(item + " ")
			outputFile.write("]")

			# compare the past two part arrays
			partStatusArray.append(partStatus)
			partStatusArrayNum = partStatusArrayNum + 1

			# get the cuepart array
			if partStatusArrayNum == 2:
				#print partStatusArray[0]
				#print partStatusArray[1]
				#partsToCue = compareLists(partStatusArray[0], partStatusArray[1])
				#print partsToCue
				partStatusArrayNum = 1
				temp = partStatusArray[1]
				partStatusArray = []
				partStatusArray.append(temp)

			partStatus = []
			partsToCue
		print "\n"

	outputFile.close()
	print "\n"
	print "Done."

main()