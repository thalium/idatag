import idaapi
import idautils
import json

tags = []

def export_tags(tags):
	out_dir = idautils.GetIdbDir() + idaapi.get_root_filename() + "_TagApi" + ".json"
	with open(out_dir, 'wb') as f:
		json.dump(tags, f)
	print("[TagApi] Tags created : " + str(len(tags)))
	print("[TagApi] Tags exported : " + out_dir)


def imp_cb(ea, name, ord):
	for xref in XrefsTo(ea, True):
		for func_ea in Functions():
			for (startea, endea) in idautils.Chunks(func_ea):
				if startea < xref.frm < endea:
					if name:
						export_tag = {'offset': (func_ea- idaapi.get_imagebase()),'tag': name, 'feeder': 'TagApi'}
						tags.append(export_tag)
	return True

nimps = idaapi.get_import_module_qty()

print "Found %d import(s)..." % nimps

for i in xrange(0, nimps):
	name = idaapi.get_import_module_name(i)
	if not name:
		print "Failed to get import module name for #%d" % i
		continue

	print "Walking-> %s" % name
	idaapi.enum_import_names(i, imp_cb)

export_tags(tags)