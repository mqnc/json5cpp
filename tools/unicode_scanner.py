def extract_ranges(file_path, target_categories):
	result_ranges = []
	current_range = None
	
	with open(file_path, 'r', encoding='utf-8') as file:
		for line in file:
			if line.strip():  # Skip empty lines
				fields = line.strip().split(';')
				
				codepoint = int(fields[0], 16)
				category = fields[2]
				
				if category in target_categories:
					if current_range is None:
						current_range = (codepoint, codepoint)
					else:
						current_range = (current_range[0], codepoint)
				elif current_range:
					result_ranges.append(current_range)
					current_range = None
	
	if current_range:
		result_ranges.append(current_range)
	
	return result_ranges

def extract_ranges(file_path, target_categories):
	result_ranges = []
	current_range = None
	
	with open(file_path, 'r', encoding='utf-8') as file:
		for line in file:
			if line.strip():  # Skip empty lines
				fields = line.strip().split(';')
				
				codepoint = int(fields[0], 16)
				category = fields[2]
				range_marker = fields[1]
				
				if category in target_categories:
					if range_marker.endswith("First>"):
						if current_range is None:
							current_range = (codepoint,)
					elif range_marker.endswith("Last>"):
						assert(current_range is not None)
						current_range = (current_range[0], codepoint)
					else:
						if current_range is None:
							current_range = (codepoint, codepoint)
						else:
							current_range = (current_range[0], codepoint)
				elif current_range:
					result_ranges.append(current_range)
					current_range = None
	
	if current_range:
		result_ranges.append(current_range)
	
	return result_ranges

file_path = 'Unicode10Data.txt'
category_start = ["Lu", "Ll", "Lt", "Lm", "Lo", "Nl"]
category_part = category_start + ["Mn", "Mc", "Nd", "Pc"]
category_space = ["Zs"]
id_start = extract_ranges(file_path, category_start)
id_part = extract_ranges(file_path, category_part)
space = extract_ranges(file_path, category_space)

for group in [id_start, id_part, space]:
	txt = ""
	for char_range in group:
		if char_range[1] <= 127:
			continue
		if char_range[0] == char_range[1]:
			txt += "|| c == " + hex(char_range[0])
		else:
			txt += f"|| (c >= {hex(char_range[0])} && c <= {hex(char_range[1])})" 
		txt += "\n"
	print(txt + "\n\n\n")
