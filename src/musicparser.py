#!/usr/bin/python
import xml.etree.ElementTree as ET
import io

# Take two lists and compare them
def compareLists(listA, listB):
	cueParts = []
	print "Parts to Cue:"
	for i in range(len(listA)):
		if listA[i] is "REST" and listB[i] is not "REST":
			cueParts.append(listB[i])
		else:
			cueParts.append("_")
	return cueParts


def main():
	# Open XML document using minidom parser
	fileName = "timewise-ActorPrelude.xml"
	#fileName = "timeWiseMarvelVsCapcom.xml"
	DOMTree = ET.parse(fileName)
	root = DOMTree.getroot()
	partStatus = []
	partStatusArray = [] #should have 2 partStatus arrays in it
	partStatusArrayNum = 0
	partsToCue = []
	dynamic = ""
	tempo = ""
	time = ""
	beats =""
	beatsType=""
	timeSignature=""

	outfile = fileName.replace(".",'')
	with open(outfile + "-output.xml", mode='wt') as outputFile:
		try:
			print "Instrument parts and ID:"
			print "========================"
			for part in root.iter('score-part'):
				print "%s" % part[0].text + ", id: %s" % part.attrib['id']

			outputFile.write("Measure Number, time signature, tempo, dynamics, entrance cues")
			print "\n"
			print "Measures:"
			print "==========="
			for measure in root.iter('measure'):

				measureNumber = measure.attrib['number']
				print "measure " + "%s" % measure.attrib['number']
				outputFile.write("\n")
				outputFile.write(measureNumber + ",")

				# attributes tag that contains the time signature (beats)
				#measureAttributeTag = measure[0][1]
				for part in measure.find('part'):
					for attribute in part.iter('attributes'):
						for timeTag in attribute.iter('time'):
							#print list(timeTag)
							for beatTag in timeTag:
								beats = timeTag.find('beats').text
								beatsType = timeTag.find('beat-type').text
								timeSignature = beats + "/" +beatsType
							print timeSignature

				outputFile.write(timeSignature + ",")

				# print measure[0] line 528
				for direction in measure.iter('direction'):
					if direction.find('sound') is not None:
						soundTag = direction.find('sound')
						if soundTag.get('tempo') is not None:
							tempo = soundTag.get('tempo')
							print "tempo " + tempo
							break

				outputFile.write(tempo + ",")

				# get dynamics tag (inside a measure) if there is one
				for part in measure: #part
					for partElem in part: #direction
						for subElems in partElem: #direction-type
							if subElems.find('dynamics') is not None:
								dynamicsTag = subElems.find('dynamics')
								dynamic = dynamicsTag[0].tag
								#print "\t" + part.attrib['id'] + " dynamic-> " + dynamic
								outputFile.write(part.attrib['id'] + "-" + dynamic + " ")
					# find all notes played by this part in current measure
					note = part.find('note')
					if note.find('rest') is not None:
						partStatus.append("REST")
					else:
						partStatus.append(part.attrib['id'])

				# compare the past two part arrays
				partStatusArray.append(partStatus)
				partStatusArrayNum = partStatusArrayNum + 1

				#outputFile.write("")

				# get the cuepart array
				if partStatusArrayNum == 2:
					#print partStatusArray[0]
					#print partStatusArray[1]
					partsToCue = compareLists(partStatusArray[0], partStatusArray[1])
					print partsToCue

					partStatusArrayNum = 1
					temp = partStatusArray[1]
					partStatusArray = []
					partStatusArray.append(temp)
					
					outputFile.write(",")
					for item in partsToCue:
						outputFile.write(item + " ")

				partStatus = []
				partsToCue = []
			
		except UnicodeEncodeError as e:
			print "UnicodeError"
			pass

		print "\n"

	outputFile.close()
	print "\n"
	print "Done."

main()