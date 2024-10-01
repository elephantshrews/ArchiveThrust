from modules import *



username = "m.snoeken@campus.tu-berlin.de"
password = "UfQx95rK5Bj4haRhiKBP"
norad_id = "25544"


login(username, password)
temp_stor = create_temporary_storage()
perm_stor = create_permanent_storage()
download(norad_id, temp_stor, perm_stor)
