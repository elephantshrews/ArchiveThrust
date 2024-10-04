from modules import *

username = "m.snoeken@campus.tu-berlin.de"
password = "UfQx95rK5Bj4haRhiKBP"
norad_id = "25544"



login(username,password)
perm_stor = create_permanent_storage()
download(norad_id,perm_stor)
