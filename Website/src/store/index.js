import Vue from 'vue'
import Vuex from 'vuex'
import getters from './getters'
import app from './modules/app'
import settings from './modules/settings'
import esp_data from './modules/esp_data'

Vue.use(Vuex)

const store = new Vuex.Store({
  modules: {
    app,
    settings,
    esp_data
  },
  getters
})

export default store
