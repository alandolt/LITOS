<template>
  <div class="app-container">

    <el-form
      ref="config_form"
      :model="config"
      label-width="180px"
      label-position="left"
      :rules="rules"
      hide-required-asterisk:true
    >

      <h2>Webinterface</h2>
      <el-form-item label="Connection mode:">
        <el-select v-model="config.webserver.mode" size="large" style="width: 100%;">
          <el-option
            v-for="item in con_mode"
            :key="item.value"
            :label="item.label"
            :value="item.value"
          />
        </el-select>
      </el-form-item>
      <div v-show="config.webserver.mode == 1">
        <el-form-item label="Access point name:" prop="AP_mode.AP_ssid">
          <el-input
            v-model="config.AP_mode.AP_ssid"
            placeholder="Name of the created access point"
          />
        </el-form-item>
        <el-form-item label="Password protected:">
          <el-switch v-model="config.AP_mode.AP_prot" />
        </el-form-item>
        <el-form-item
          v-show="config.AP_mode.AP_prot"
          label="Access point password:"
          prop="AP_mode.AP_password"
        >
          <el-input
            v-model="config.AP_mode.AP_password"
            show-password
            placeholder="Please enter a password to protect the created access point"
          />
        </el-form-item>
      </div>
      <div v-show="config.webserver.mode == 2">
        <el-form-item label="Wi-Fi SSID:" prop="WPA_mode.ssid">
          <el-input
            v-model="config.WPA_mode.ssid"
            placeholder="Please enter the name (SSID) of the Wi-Fi network"
          />
        </el-form-item>
        <el-form-item label="Wi-Fi password:" prop="WPA_mode.wpa_password">
          <el-input
            v-model="config.WPA_mode.wpa_password"
            show-password
            placeholder="Please enter the password of the Wi-Fi network"
          />
        </el-form-item>
      </div>
      <div v-show="config.webserver.mode == 3">
        <el-form-item label="Eduroam identity:" prop="EAP_mode.EAP_identity">
          <el-input
            v-model="config.EAP_mode.EAP_identity"
            placeholder="Please enter your username (e.g. username@youruniversity.countrycode)"
          />
        </el-form-item>
        <el-form-item label="Eduroam password:" prop="EAP_mode.EAP_password">
          <el-input
            v-model="config.EAP_mode.EAP_password"
            placeholder="Please enter your password"
          />
        </el-form-item>
      </div>
      <el-form-item label="Advanced settings:">
        <el-switch v-model="config.webserver.adv_set" />
      </el-form-item>
      <div v-show="config.webserver.adv_set">
        <el-form-item label="Hostname:" prop="webserver.hostname">
          <el-input
            v-model="config.webserver.hostname"
            placeholder="The hostname can be used instead of the IP address (only in WPA mode)"
          />
        </el-form-item>
        <el-form-item label="Webinterface port:" prop="webserver.port" min="1" max="65535">
          <el-input-number v-model="config.webserver.port" />
        </el-form-item>
      </div>
      <h2>LED Matrix</h2>
      <el-form-item label="LED offset:">
        <el-switch v-model="config.mat_cor.act" />
      </el-form-item>
      <el-form
        v-if="config.mat_cor.act"
        :inline="true"
        label-width="110px"
        label-position="left"
        style="padding-left: 180px"
      >
        <el-form-item label="Pixel offset x:">
          <el-input-number v-model="config.mat_cor.x" :min="-10" :max="10" />
        </el-form-item>
        <el-form-item label="Pixel offset y:" style="padding-left=30px">
          <el-input-number v-model="config.mat_cor.y" :min="-10" :max="10" />
        </el-form-item>
      </el-form>
      <el-form-item label="Outline color:">
        <el-color-picker v-model="config.mark_col" color-format="rgb" :predefine="predefineColors" />
      </el-form-item>
      <h2>About</h2>
      <el-form-item label="Software version:">
        {{ config.vers }}
      </el-form-item>
      <el-form-item>
        <el-button type="primary" @click="submitForm('config_form')">Save Settings</el-button>
        <el-button @click="resetForm()">Cancel</el-button>
      </el-form-item>
    </el-form>
  </div>
</template>

<script>
import Socket from '@/socket'
// import { clone, cloneDeep } from 'lodash'
import { transform, isEqual, isObject, isEmpty } from 'lodash'

/*
function diff(tgt, src) {
  // if you got object
  var rst = {}
  for (var k in tgt) {
    // visit all fields
    if (typeof src[k] === 'object') {
      // if field contains object (or array because arrays are objects too)
      rst[k] = diff(tgt[k], src[k]) // diff the contents
    } else if (src[k] !== tgt[k]) {
      // if field is not an object and has changed
      rst[k] = tgt[k] // use new value
    }
    // otherwise just skip it
  }
  return rst
}*/

function difference(object, base) {
  function changes(object, base) {
    return transform(object, function(result, value, key) {
      if (!isEqual(value, base[key])) {
        result[key] =
          isObject(value) && isObject(base[key])
            ? changes(value, base[key])
            : value
      }
    })
  }
  return changes(object, base)
}

const e_AP_ssid = 'Please enter an access point name'
const e_AP_password =
  'Please enter a password for your access point (min 8 characters)'
const e_wpa_ssid = 'Please enter the name of the network'
const e_wpa_password =
  'Please enter the password (min 8 characters)'
const e_eap_identity = 'Please enter your identity for the eduroam network'
const e_eap_password =
  'Please enter your password'
const e_hostname = 'Please enter a valid hostname'
const e_port = 'Please enter a valid port LITOS\'s webserver'

export default {
  data() {
    var check_AP = (rule, value, callback) => {
      if (this.config.webserver.mode === 1 && value === '' && value === ' ') {
        return callback(new Error())
      } else {
        callback()
      }
    }
    var check_WPA = (rule, value, callback) => {
      if (this.config.webserver.mode === 2 && value === '' && value === ' ') {
        return callback(new Error())
      } else {
        callback()
      }
    }
    var check_EAP = (rule, value, callback) => {
      if (this.config.webserver.mode === 3 && value === '' && value === ' ') {
        return callback(new Error())
      } else {
        callback()
      }
    }
    var AP_password = (rule, value, callback) => {
      if (
        this.config.webserver.mode === 1 &&
        value === '' &&
        this.config.AP_mode.AP_prot
      ) {
        return callback(new Error())
      } else {
        callback()
      }
    }

    return {
      color: 'rgb(12,12,12)',
      predefineColors: [
        'rgb(255, 0, 0)',
        'rgb(0, 255, 0)',
        'rgb(255, 255, 255)',
        'rgb(0, 0, 255)'
      ],
      con_mode: [
        {
          value: 1,
          label: 'Access point mode'
        },
        {
          value: 2,
          label: 'WPA / WPA2 Wi-Fi Network (WLAN with password)'
        },
        {
          value: 3,
          label: 'Eduroam network (still in testing)'
        }
      ],
      rules: {
        'AP_mode.AP_ssid': [
          {
            validator: check_AP,
            trigger: 'blur',
            message: e_AP_ssid
          }
        ],
        'AP_mode.AP_password': [
          {
            validator: AP_password,
            trigger: 'blur',
            message: e_AP_password
          },
          {
            min: 8,
            max: 32,
            message: 'Length of the password should be 8 to 32 characters',
            trigger: 'blur'
          }
        ],
        'WPA_mode.ssid': [
          {
            validator: check_WPA,
            trigger: 'blur',
            message: e_wpa_ssid
          }
        ],
        'WPA_mode.wpa_password': [
          {
            validator: check_WPA,
            trigger: 'blur',
            message: e_wpa_password
          },
          {
            min: 8,
            max: 32,
            message: 'Length of the password should be 8 to 32 digits',
            trigger: 'blur'
          }
        ],
        'EAP_mode.EAP_identity': [
          {
            validator: check_EAP,
            trigger: 'blur',
            message: e_eap_identity
          }
        ],
        'EAP_mode.EAP_password': [
          {
            validator: check_EAP,
            trigger: 'blur',
            message: e_eap_password
          }
        ],
        'webserver.hostname.': [
          {
            required: true,
            trigger: 'blur',
            message: e_hostname
          }
        ],
        'webserver.port': [
          {
            trigger: 'blur',
            message: e_port
          }
        ]
      },
      config: {
        response: null,
        vers: null,
        webserver: {
          port: 0,
          hostname: null,
          mode: 1,
          adv_set: false
        },
        WPA_mode: {
          ssid: null,
          wpa_password: null
        },
        AP_mode: {
          AP_prot: false,
          AP_ssid: null,
          AP_password: null
        },
        EAP_mode: {
          EAP_identity: null,
          EAP_password: null
        },
        mat_cor: {
          act: false,
          x: 0,
          y: 0
        },
        mark_col: null
      }
    }
  },
  mounted() {
    Socket.send(JSON.stringify({ command: 'get_settings' }))
    setTimeout(() => {
      this.config = Object.assign(
        this.config,
        JSON.parse(JSON.stringify(this.$store.state.esp_data.config))
      )
      // Object.assign(this.config, this.$store.state.esp_data.config);
    }, 1000)
  },
  methods: {
    resetForm() {
      this.config = Object.assign(
        this.config,
        JSON.parse(JSON.stringify(this.$store.state.esp_data.config))
      )
    },
    submitForm(formName) {
      this.$refs.config_form.validate(valid => {
        if (valid) {
          var obj_difference = difference(
            this.config,
            this.$store.state.esp_data.config
          )
          if (isEmpty(obj_difference)) {
            return false
          }
          obj_difference['command'] = 'update_settings'
          var obj_json = JSON.stringify(obj_difference, (key, value) => {
            if (value !== null) return value
          })
          Socket.send(obj_json)
          this.$store.commit('save_settings', {
            obj: this.config
          })
          Socket.send(JSON.stringify({ command: 'get_settings' }))
          setTimeout(() => {
            this.config = Object.assign(
              this.config,
              JSON.parse(JSON.stringify(this.$store.state.esp_data.config))
            )
          }, 1000)
          this.$alert(
            'The settings were sucessfull changed. LITOS will now reboot, please wait for about 30s <br> Maybe you need to reconnect with the LITOS access point.',
            'Settings changed',
            {
              dangerouslyUseHTMLString: true,
              confirmButtonText: 'OK',
              callback: action => {
                this.$message({
                  type: 'success',
                  message: `New settings were loaded`
                })
              }
            }
          )
        } else {
          console.log('error submit!!')
          return false
        }
      })
    }
  }
}
</script>

<style scoped>
.line {
  text-align: center;
}
</style>

