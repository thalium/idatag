import idaapi
import idautils
import json

tags = []

def export_tags(tags):
	out_dir = idautils.GetIdbDir() + idaapi.get_root_filename() + "_TagFuncRefs" + ".json"
	with open(out_dir, 'w') as f:
		json.dump(tags, f)
	print("[TagFuncRefs] Tags created : " + str(len(tags)))
	print("[TagFuncRefs] Tags exported : " + out_dir)

inf = idaapi.get_inf_structure()

for func_ea in idautils.Functions(inf.min_ea, inf.max_ea):
	func = ida_funcs.get_func(func_ea)
	func_name = get_name(func_ea)
	for ea in idautils.FuncItems(func.start_ea):
		xref_name = ""
		for xref in XrefsFrom(ea, False):
			if xref.type == fl_CF or xref.type ==fl_CN:
				export_tag = {'offset': (func_ea - idaapi.get_imagebase()),'tag': get_name(xref.to), 'feeder': 'TagFuncRefs'}
				tags.append(export_tag)
export_tags(tags)