# get tempo

import xml.etree.ElementTree as ET
import io

# Open XML document using minidom parser
fileName = "timewise-ActorPrelude.xml"
DOMTree = ET.parse(fileName)
root = DOMTree.getroot()

print "\n"
print "Measures:"
print "==========="
# interested in tags direction and direction-type
for measure in root.iter('measure'):
	measure_str = "measure " + "%s" % measure.attrib['number']
	print measure_str
	for direction in measure.iter('direction'):
		if direction.find('direction-type') is None:
			continue
		else:
			directionType = direction.findall('direction-type')
			for m in directionType: 
				metronome = m.find('metronome')
				if metronome is not None:
					for tags in metronome:
						print "\t" + tags.text


print "\n"
print "Done."