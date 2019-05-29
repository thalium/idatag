import json
import sys
import os

def get_offset(call):
    s = call.split("+")
    try:
        return int(s[1], 16)
    except:
        pass

def get_hashsum(call):
    s = call.split("{")
    try:
        ss = s[1].split("}")
        return ss[0]
    except:
        s = call.split("!")
        try:
            return s[0]
        except:
            return

def generate_tag(call, proto):
	offset = get_offset(call)
	if offset is None:
		return
	hashsum = get_hashsum(call)
	if hashsum is None:
		return
	return {'offset': offset, 'tag': proto, 'feeder': 'TagPcap', 'hash': hashsum}

def export_tags(tag_list, jsonf):
	out_f = jsonf + "_TagPcap" + ".json"
	with open(out_f, 'wb') as f:
		json.dump(tag_list, f)
	print("[TagPcap] Tags created : " + str(len(tag_list)))
	print("[TagPcap] Tags exported : " + out_f)

def usage():
    print("Usage : TagPcap.py <JSON PCAP file>")

def main(argv):
    try:
        jsonf = sys.argv[1]
    except:
        usage()
        return

    tag_list = []
    with open(jsonf, "r") as f:
        json_data = json.load(f)
        for l0 in json_data:
            try:
                callstack = l0["_source"]["layers"]["pkt_comment"]["frame.comment"]
                calls = callstack.splitlines()
                proto = l0["_source"]["layers"]["frame"]["frame.protocols"]
            except:
                continue
            for call in calls:
				tag = generate_tag(call, proto)
				if tag is not None:
					tag_list.append(tag)

    export_tags(tag_list, jsonf)


if __name__ == "__main__":
    main(sys.argv[1:])
