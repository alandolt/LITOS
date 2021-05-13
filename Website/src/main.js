import Vue from 'vue'

import 'normalize.css/normalize.css' // A modern alternative to CSS resets

// import locale from 'element-ui/lib/locale/lang/en' // lang i18n
import {
  Button, Select, Option,
  OptionGroup, Breadcrumb, BreadcrumbItem, Form, FormItem, Menu,
  Submenu,
  MenuItem,
  MenuItemGroup,
  Switch,
  Input, InputNumber, Message, Icon, Upload, TableColumn, Table,
  MessageBox, Alert,
  ColorPicker
} from 'element-ui'
import lang from 'element-ui/lib/locale/lang/en'
import locale from 'element-ui/lib/locale'
import '@/styles/index.scss' // global css

import App from './App'
import store from './store'
import router from './router'

import '@/icons' // icon

locale.use(lang)
// const _Loading = Loading
// const { directive: loadingDirective, service: loadingService } = _Loading
const _MessageBox = MessageBox
// const { alert, confirm, prompt } = _MessageBox
const { alert } = _MessageBox

Vue.use(Button)
Vue.use(Select)
Vue.use(Option)
Vue.use(OptionGroup)
Vue.use(Breadcrumb)
Vue.use(BreadcrumbItem)
Vue.use(Form)
Vue.use(FormItem)
Vue.use(Menu)
Vue.use(Submenu)
Vue.use(MenuItem)
Vue.use(MenuItemGroup)
Vue.use(Switch)
Vue.use(Input)
Vue.use(InputNumber)
Vue.use(Icon)
Vue.use(Upload)
Vue.use(TableColumn)
Vue.use(Table)
Vue.use(Alert)
Vue.use(ColorPicker)

Vue.prototype.$msgbox = _MessageBox
Vue.prototype.$alert = alert
Vue.prototype.$message = Message

/**
 * If you don't want to use mock-server
 * you want to use MockJs for mock api
 * you can execute: mockXHR()
 *
 * Currently MockJs will be used in the production environment,
 * please remove it before going online ! ! !
 */

// set ElementUI lang to EN
// 如果想要中文版 element-ui，按如下方式声明
// Vue.use(ElementUI);
/* Vue.use(Select);
Vue.use(Breadcrumb);
Vue.use(BreadcrumbItem);
Vue.use(Input);
Vue.use(InputNumber);
Vue.use(Row);
Vue.use(Col);

*/
Vue.config.productionTip = false

new Vue({
  el: '#app',
  router,
  store,
  render: h => h(App)
})
