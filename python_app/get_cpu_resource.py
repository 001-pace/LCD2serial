import psutil
import serial  
import serial.tools.list_ports 
import time
import random

def bytes2human(n):
	"""
	>>>bytes2human(1024)
	1.0K/s
	"""
	symbols = ('K', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y')
	prefix = {}
	for i, s in enumerate(symbols):
		prefix[s] = 1<<((i+1)*10)
	
	for s in reversed(symbols):
		if n >= prefix[s]:
			value = float(n)/prefix[s]
			return '%.1f%s' % (value, s)
	return '%dB' % (n)

def get_time():
	return time.strftime("%H:%M %m/%d/%Y", time.localtime())
		
def get_cpu_mem_info():
	cpuinfo =  psutil.cpu_percent(interval=1)
	memoryinfo = psutil.virtual_memory().percent
	return 'CPU:%d%% MEM:%d%%' % (cpuinfo, memoryinfo)

def get_network_info():
	pnic1 = psutil.net_io_counters()
	time.sleep(1)	# one second
	pnic2 = psutil.net_io_counters()
	
	str_tp = '\xfe' + str(bytes2human(pnic2.bytes_recv -pnic1.bytes_recv)) + '/s ' 	#  0xfe is customer char '↓'
	str_tp += '\xff' + str(bytes2human(pnic2.bytes_sent-pnic1.bytes_sent)) + '/s'	#  0xff is customer char '↑'
	return str_tp

def get_disk_info(n):
	disks = psutil.disk_partitions()
	str = ''
	for i,s in enumerate(disks):
		if(n%2 ==0):
			#windows
			str +='%s:'% s.device[0]+'%.1f%%(Used) ' % psutil.disk_usage(s.device).percent
			#linux
			#print('%s:'% s.device[(len(s.device)-4):len(s.device)]+'%.1f%%(U) ' % psutil.disk_usage(s.device).percent)
		else:
			#windows
			str +='%s:'% s.device[0]+ '%s(Free) ' %  bytes2human(psutil.disk_usage(s.device).free)
			#linux
			#print ('%s:'% s.device[(len(s.device)-4):len(s.device)]+ '%s(F)' %  bytes2human(psutil.disk_usage(s.device).free))
	return str
	
def get_netcard():
	netcard_info = []
	info = psutil.net_if_addrs()
	for k,v in info.items():
		for item in v:
			if item[0] == 2 and not item[1]=='127.0.0.1':
				netcard_info.append((k,item[1]))

	return netcard_info

try:
	# ------------------- user config section ------------------------------------------------------
	refresh_cycle_time = 5		#refresh time, by every XX second.
	display_cpu_level = 1		#cpu&mem info refresh times level, numbaer 1 is highest level, 1+ is lower level
	display_netwk_level = 1		#network speed info refresh times level, numbaer 1 is highest level, 1+ is lower level
	display_ip_level = 6		#ip address info refresh times level, numbaer 1 is highest level, 1+ is lower level
	display_hdd_level = 7		#hdd info refresh times level, numbaer 1 is highest level, 1+ is lower level
	display_date_level = 12		#date info refresh times level, numbaer 1 is highest level, 1+ is lower level
	
	my_netwk_card = 'WLAN 3'	#set network adapter name by user
	
	bps = 115200		#serial speed value
	timex = None
	# ------------------------------ end -------------------------------------------------------------

	port_list = list(serial.tools.list_ports.comports())
	if len(port_list) == 0:
		print('Serials no found, check hardware!!')
	else:
		ser = serial.Serial(list(port_list[len(port_list)-1])[0], bps, timeout=timex)
		isetup=1
		while 1:
			str1line = ''	
			str2line = ''
			
			#get cpu and memory info
			if (isetup%display_cpu_level==0):
				str1line = get_cpu_mem_info()
			
			#get network speed info
			if (isetup%display_netwk_level==0):
				str2line = get_network_info()
			
			#get disk info
			if (isetup%display_hdd_level==0):
				dinfo = get_disk_info(round(random.uniform(1, 2)))
				dinfo_list = dinfo[:-1].split(' ')	#cut last space char, then get disk list.
				str2line =  dinfo_list[round(random.uniform(0, len(dinfo_list)-1))]# random display one of disk info list. 
			
			#get id address info
			if (isetup%display_ip_level==0):
				ntf = get_netcard()	#get local ip address
				for k,v in ntf:
					if k==my_netwk_card:
						str1line = v
			
			#get date info
			if (isetup%display_date_level==0):
				str2line=get_time()

			# -------------------------package data section----------------------------------------
			"""
				data[35-bytes] =  { 0xff(1-byte) , 
									first line data(16-bytes data),  
									first second data(16-bytes data),   
									line number (1-byte) ,
									bytes sum check number(1-byte)}
			"""
			# package header
			result = ser.write(b'\xff')
			
			sum_ck = 0
			# package first line data
			for j in range(0, 16):
				if j==len(str1line)-1:
					ser.write(str1line.encode('ascii'))
				if (j>=len(str1line)):	#if str no char, full space char
					ser.write(b'\x20')
					sum_ck += 0x20
				else:
					sum_ck += ord(str1line[j])
					
			# package second line data
			for j in range(0, 16):
				if len(str2line)>0 and (j<len(str2line)):
					bch = str2line[j]
					if bch=='\xff':
						ser.write(b'\xff')
						sum_ck += 0xff
					elif bch=='\xfe':
						ser.write(b'\xfe')
						sum_ck += 0xfe
					else:
						ser.write(str2line[j].encode('ascii'))
						sum_ck += ord(str2line[j])
				else:	#if str no char, full space char
					ser.write(b'\x20')
					sum_ck += 0x20
			
			# package sent lines info  0x00 mean one line, 0x01 mean two line
			if(len(str2line) == 0):
				ser.write(b'\x00')
			else:
				ser.write(b'\x01')
				sum_ck+=0x01
				
			#format package sum data, than sent it.
			sum_ck&=0xff
			str_temp = '%02x'%sum_ck
			ser.write(bytes.fromhex('%02x'%sum_ck))
			# ------------------------- end ----------------------------------------
			
			# ---------get work result ------------
			lcd_statu = int(ser.read().hex())
			if(lcd_statu != 0x01):
				print('display fail ' + get_time())
			#-------------------------------------
			
			isetup+=1;
			if(isetup>20000):
				isetup=1
			time.sleep(refresh_cycle_time)

except Exception as e:
     print("---Error---:",e)