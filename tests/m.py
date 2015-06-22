import datetime

import msgpack

mev_dict = {
    "v": 1,
    "r": datetime.datetime.now(),
    "t": datetime.datetime.now(),
}

def decode_datetime(obj):
    if b'__datetime__' in obj:
        obj = datetime.datetime.strptime(obj["as_str"], "%H%M%S")
    return obj

def encode_datetime(obj):
    if isinstance(obj, datetime.datetime):
        return {'__datetime__': True, 'as_str': obj.strftime("%H%M%S")}
    return obj


packed_dict = msgpack.packb(mev_dict, default=encode_datetime)

print packed_dict,'\n'


this_dict_again = msgpack.unpackb(packed_dict, object_hook=decode_datetime)
print this_dict_again,'\n'
