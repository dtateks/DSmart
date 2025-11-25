import string

# -------------------------------------------------------------------------
# Auto-Sync Topic with DeviceName
# -------------------------------------------------------------------------
def sync_topic()
    print("AUTO-SYNC: Starting check...")
    
    # Get DeviceName
    var res_dn = tasmota.cmd("DeviceName")
    if res_dn == nil
        print("AUTO-SYNC: Error - DeviceName response is nil")
        return
    end
    if !res_dn.contains("DeviceName")
        print("AUTO-SYNC: Error - Response does not contain DeviceName")
        return
    end
    var dev_name = res_dn["DeviceName"]
    print(string.format("AUTO-SYNC: DeviceName is '%s'", dev_name))
    
    # Get Current Topic
    var res_top = tasmota.cmd("Topic")
    if res_top == nil
        print("AUTO-SYNC: Error - Topic response is nil")
        return
    end
    if !res_top.contains("Topic")
        print("AUTO-SYNC: Error - Response does not contain Topic")
        return
    end
    var cur_topic = res_top["Topic"]
    print(string.format("AUTO-SYNC: Current Topic is '%s'", cur_topic))
    
    # Sync if different
    if dev_name != cur_topic
        print(string.format("AUTO-SYNC: Updating Topic (%s) -> (%s)", cur_topic, dev_name))
        tasmota.cmd("Topic " + dev_name)
    else
        print("AUTO-SYNC: Topic is already synchronized.")
    end
end

# Trigger sync when Wifi connects (with delay to ensure system is ready)
tasmota.add_rule("Wifi#Connected", def() tasmota.set_timer(2000, sync_topic) end)

# Trigger sync when Ethernet connects
tasmota.add_rule("Eth#Connected", def() tasmota.set_timer(2000, sync_topic) end)

# Check if already connected at startup (Wifi or Ethernet)
var connected = false
if tasmota.wifi().find("ip") connected = true end
try
    if tasmota.eth().find("ip") connected = true end
except .. end

if connected
    print("AUTO-SYNC: Network already connected, scheduling sync...")
    tasmota.set_timer(2000, sync_topic)
end
