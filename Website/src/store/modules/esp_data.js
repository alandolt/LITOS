
const state = {
  file_list: [
    {
      file: '/conf/a',
      filename: 'a'
    },
    {
      file: '/conf/b',
      filename: 'b'
    }
  ],
  config: {
    response: 'get_settings',
    vers: null,
    webserver:
    {
      port: null,
      hostname: null,
      mode: 1,
      adv_set: false
    },
    WPA_mode:
    {
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
      x: null,
      y: null
    },
    mark_col: null
  }
}

const mutations = {
  add_file(state, payload) {
    state.file_list.push({
      file: payload.file,
      filename: payload.filename
    })
  },
  delete_file_list(state) {
    state.file_list = []
  },
  delete_one_file(state, payload) {
    state.file_list = state.file_list.filter(function(el) { return el.file !== payload.file_to_be_deleted })
  },
  save_settings(state, payload) {
    // Object.assign(state.config, payload.obj)<
    state.config = JSON.parse(JSON.stringify(payload.obj))
  }
}

const actions = {

}

export default {
  state,
  mutations,
  actions
}
