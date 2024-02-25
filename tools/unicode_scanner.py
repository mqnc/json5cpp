filePath = 'tools/Unicode10Data.txt'

categories = []

lastCodepoint = -1

with open(filePath, 'r', encoding = 'utf-8') as file:
	for line in file:
		if line.strip(): # Skip empty lines
			fields = line.strip().split(';')

			codepoint = int(fields[0], 16)
			name = fields[1]
			category = fields[2]

			if codepoint > lastCodepoint + 1:
				for cp in range(lastCodepoint + 1, codepoint):
					if name.endswith("Last>"):
						# inside a group of characters
						categories.append(category)
					else:
						# inside a gap (noncharacters or unassigned)
						categories.append(None)
			categories.append(category)
		lastCodepoint = codepoint
	for cp in range(len(categories), 0x110000):
		categories.append("-")

def extractRanges(targetCategories):
	ranges = []
	currentRange = None
	for cp, category in enumerate(categories):
		if category in targetCategories:
			if currentRange is None:
				currentRange = (cp, cp)
			else:
				currentRange = (currentRange[0], cp)
		else:
			if currentRange is not None:
				ranges.append(currentRange)
				currentRange = None
	return ranges

category_start = ["Lu", "Ll", "Lt", "Lm", "Lo", "Nl"]
category_cont_extra = ["Mn", "Mc", "Nd", "Pc"]
category_cont = category_start + category_cont_extra
category_space = ["Zs"]
id_start = extractRanges(category_start)
id_cont = extractRanges(category_cont)
id_cont_extra = extractRanges(category_cont_extra)
space = extractRanges(category_space)

for label, group in [("start", id_start), ("cont", id_cont), ("cont_extra", id_cont_extra), ("space", space)]:
	print(label + "\n\n")
	txt = ""
	col=0
	for char_range in group:
		if char_range[1] <= 127:
			continue
		if char_range[0] == char_range[1]:
			txt += "|| c == " + hex(char_range[0])
		else:
			txt += f"|| (c >= {hex(char_range[0])} && c <= {hex(char_range[1])})"
		col += 1
		if col % 4 == 0:
			txt += "\n"
		else:
			txt += " "
	print(txt + "\n\n\n")
