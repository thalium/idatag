import idaapi
import idc
import idautils
import json
import re


def export_tags(tags):
	out_dir = idautils.GetIdbDir() + idaapi.get_root_filename() + "_TagSource" + ".json"
	with open(out_dir, 'wb') as f:
		json.dump(tags, f)
	print("[TagSource] Tags created : " + str(len(tags)))
	print("[TagSource] Tags exported : " + out_dir)

def get_sources():
	tags = []
	export_tag = {}
	string_refs = {}
	strings_ida = idautils.Strings(False)
	strings_ida.setup(strtypes=[idc.STRTYPE_C, idc.STRTYPE_C_16])
	r = re.compile(r".*\.(c|cpp|h|hpp)")

	for string in strings_ida:
		string_refs[string.__str__()] = string.ea

	strings = string_refs.keys()
	matched = filter(r.match, strings)
	for string in matched:
		export_tag = {'offset': (string_refs[string] - idaapi.get_imagebase()),
					  'tag': 'Source', 'feeder': 'TagSource'}
		tags.append(export_tag)
	export_tags(tags)

def main():
	get_sources()
	
if __name__ == "__main__":
    main()
