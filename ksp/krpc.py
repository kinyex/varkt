import krpc
import time

angleSpreadsheet = [0, 0, 15, 5.55, 25, 22.49, 50, 33.67, 75, 48.08, 100, 53.98, 125, 57.82, 150, 60.23, 200, 65.56, 300, 66.65, 400, 72.3, 550, 95]

def getAngle(time):
	if time <= angleSpreadsheet[0]: return angleSpreadsheet[1]
	if time >= angleSpreadsheet[-2]: return angleSpreadsheet[-1]
	for i in range(len(angleSpreadsheet)):
		if time <= angleSpreadsheet[i]:
			p = (angleSpreadsheet[i-2], angleSpreadsheet[i-1])
			k = (time - p[0]) / (angleSpreadsheet[i] - p[0])
			return p[1] * (1 - k) + angleSpreadsheet[i + 1] * k
	return 0

throttlingSpreadsheet = [0, .975, 25, .975, 25.5, 1, 53, 1, 53.1, .8, 59.9, .8, 60, .9, 59, .9, 109, .9, 110, .94, 134, .94, 147.5, .8, 150, 0, 158, 0, 164, .985, 495, .985, 525.5, .737, 530, 0, 2000, 0, 2000.1, 1, 2001.3, 1, 2001.4, 0, 3600, 0, 3601, 1, 1e13, 1, 1e14, 0]

def getThrottling(time):
	if time <= throttlingSpreadsheet[0]: return throttlingSpreadsheet[1]
	if time >= throttlingSpreadsheet[-2]: return throttlingSpreadsheet[-1]
	for i in range(len(throttlingSpreadsheet)):
		if time <= throttlingSpreadsheet[i]:
			p = (throttlingSpreadsheet[i-2], throttlingSpreadsheet[i-1])
			k = (time - p[0]) / (throttlingSpreadsheet[i] - p[0])
			return p[1] * (1 - k) + throttlingSpreadsheet[i + 1] * k
	return 0


conn = krpc.connect(name='Launch')
vessel = conn.space_center.active_vessel

vessel.control.sas = False
vessel.control.rcs = False
vessel.control.throttle = 1.0

surface_frame = vessel.orbit.body.reference_frame

streams = {
	'met': conn.add_stream(getattr, vessel, 'met'),
	'altitude': conn.add_stream(getattr, vessel.flight(), 'mean_altitude'),
	'speed': conn.add_stream(getattr, vessel.flight(reference_frame=surface_frame), 'speed'),
	'vertical_speed': conn.add_stream(getattr, vessel.flight(reference_frame=surface_frame), 'vertical_speed'),
	'horizontal_speed': conn.add_stream(getattr, vessel.flight(reference_frame=surface_frame), 'horizontal_speed'),
	'thrust': conn.add_stream(getattr, vessel, 'thrust'),
	'mass': conn.add_stream(getattr, vessel, 'mass'),
	'dynamic_pressure': conn.add_stream(getattr, vessel.flight(), 'dynamic_pressure'),
	'drag': conn.add_stream(getattr, vessel.flight(reference_frame=surface_frame), 'drag'),
	'aerodynamic_force': conn.add_stream(getattr, vessel.flight(vessel.surface_velocity_reference_frame), 'aerodynamic_force')
}

met = streams["met"]
altitude = streams["altitude"]

data = dict()
for key in streams:
	data[key] = []
data['aerodynamic_force_drag'] = []
data['aerodynamic_force_lift'] = []
data['aerodynamic_force_z'] = []

vessel.control.activate_next_stage()

latitude = 61.5

vessel.auto_pilot.engage()
vessel.auto_pilot.target_pitch_and_heading(90, latitude)

resources = vessel.resources_in_decouple_stage(vessel.control.current_stage, cumulative=False)

body = vessel.orbit.body
print(f'Planet name: {body.name}')
print(f'Planet mass: {body.mass}')

stage = 1

while met() < 5000: # and met() < 3600 / 3.39:
	tm = met() * 3.39
	if vessel.thrust == 0 and False:
		vessel.control.activate_next_stage()
		time.sleep(.05)
	
	if stage == 1 and tm > 151:
		stage = 1.5
		vessel.control.activate_next_stage()
		vessel.control.activate_next_stage()
	elif stage == 1.5 and tm > 1950:
		vessel.auto_pilot.disengage()
		vessel.control.sas = True
		time.sleep(.1)
		vessel.control.sas_mode = conn.space_center.SASMode.prograde
		stage = 2
	elif stage == 2 and tm > 3500:
		stage += 1
	elif stage == 3 and streams['altitude']() >= 100_000:
		stage += 1

	if stage != 4: vessel.control.throttle = getThrottling(tm)
	vessel.auto_pilot.target_pitch_and_heading(90 - getAngle(tm), latitude)
	
	time.sleep(.1)
	print(f'{tm:.1f}, a: {getAngle(tm):.1f}, t: {getThrottling(tm):4.3f}, {altitude():.1f}')
	for key in streams:
		if key == 'drag':
			drag = streams[key]()
			data[key].append((drag[0]**2 + drag[1]**2 + drag[2]**2) ** 0.5)
			continue
		if key == 'aerodynamic_force':
			data['aerodynamic_force_drag'].append(-streams[key]()[1])
			data['aerodynamic_force_lift'].append(streams[key]()[0])
			data['aerodynamic_force_z'].append(streams[key]()[2])
		data[key].append(streams[key]())

vessel.control.throttle = 0.0
vessel.auto_pilot.disengage()
vessel.control.sas = True

print('Done')
