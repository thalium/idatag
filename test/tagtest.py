import idaapi
import idc
import idautils
import json
import string
import random

PLUGIN_VERSION = "0.0.1"
AUTHORS = "Arnaud Gatignol @ Thalium"
DATE = "2019"


class tagstring_t(idaapi.plugin_t):
    flags = idaapi.PLUGIN_UNL
    comment = "IDATag client - Test script"
    help = ""
    wanted_name = "TagTest"
    wanted_hotkey = ""

    def __init__(self):
        super(tagstring_t, self).__init__()
        self.tags = []
        self.path = idautils.GetIdbDir() + "tags\\"
        self.base = idaapi.get_imagebase()

    def init(self):
        return idaapi.PLUGIN_OK

    def run(self, arg):
        print("=" * 80)
        print(
            "TagTest v{0} - {1} - {2}".format(PLUGIN_VERSION, AUTHORS, DATE))
        print("=" * 80)

        self.get_testtags()
        self.export_tags()

    def get_testtags(self):
        export_tag = {}
        functions = idautils.Functions()
        for function in functions:
            for i in range(10):
                feeder = ''.join(random.choice(string.ascii_uppercase + string.digits) for _ in range(4))
                tag = ''.join(random.choice(string.ascii_uppercase + string.digits) for _ in range(10))
                export_tag = {'offset': function, 'tag': tag, 'feeder': feeder}
                self.tags.append(export_tag)
        for address, _ in idautils.Names():
            for i in range(10):
                feeder = ''.join(random.choice(string.ascii_uppercase + string.digits) for _ in range(4))
                tag = ''.join(random.choice(string.ascii_uppercase + string.digits) for _ in range(10))
                export_tag = {'offset': address, 'tag': tag, 'feeder': feeder}
                self.tags.append(export_tag)

    def export_tags(self):
        out_dir = self.path + idaapi.get_root_filename() + "_TagTest" + ".json"
        with open(out_dir, 'wb') as f:
            json.dump(self.tags, f)
        print("[TagTest] Tags created : " + str(len(self.tags)))
        print("[TagTest] Tags exported : " + out_dir)

    def term(self):
        pass


def PLUGIN_ENTRY():
    return tagstring_t()
