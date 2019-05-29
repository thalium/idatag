import idaapi
import idc
import idautils
import json


def export_tags(tags):
	out_dir = idautils.GetIdbDir() + idaapi.get_root_filename() + "_TagString" + ".json"
	with open(out_dir, 'wb') as f:
		json.dump(tags, f)
	print("[TagString] Tags created : " + str(len(tags)))
	print("[TagString] Tags exported : " + out_dir)

def get_strings():
	tags = []
	export_tag = {}
	strings_ida = idautils.Strings(False)
	strings_ida.setup(strtypes=[idc.STRTYPE_C, idc.STRTYPE_C_16])

	for string in strings_ida:
		export_tag = {'offset': (string.ea - idaapi.get_imagebase()),
					  'tag': 'String', 'feeder': 'TagString'}
		tags.append(export_tag)
	export_tags(tags)

def main():
	get_strings()
	
if __name__ == "__main__":
    main()
