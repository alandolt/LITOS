import Vue from 'vue'
import Router from 'vue-router'

Vue.use(Router)

// import Dashboard from '@/views/dashboard/index'
import One_plate from '@/views/load_config/one_plate'
import Two_plates from '@/views/load_config/two_plates'
import File_manager from '@/views/file_manager/index'
import Settings from '@/views/settings/index'

/* Layout */
import Layout from '@/layout'

/**
 * Note: sub-menu only appear when route children.length >= 1
 * Detail see: https://panjiachen.github.io/vue-element-admin-site/guide/essentials/router-and-nav.html
 *
 * hidden: true                   if set true, item will not show in the sidebar(default is false)
 * alwaysShow: true               if set true, will always show the root menu
 *                                if not set alwaysShow, when item has more than one children route,
 *                                it will becomes nested mode, otherwise not show the root menu
 * redirect: noRedirect           if set noRedirect will no redirect in the breadcrumb
 * name:'router-name'             the name is used by <keep-alive> (must set!!!)
 * meta : {
    roles: ['admin','editor']    control the page roles (you can set multiple roles)
    title: 'title'               the name show in sidebar and breadcrumb (recommend set)
    icon: 'svg-name'             the icon show in the sidebar
    breadcrumb: false            if set false, the item will hidden in breadcrumb(default is true)
    activeMenu: '/example/list'  if set path, the sidebar will highlight the path you set
  }
 */

/**
 * constantRoutes
 * a base page that does not have permission requirements
 * all roles can be accessed
 */
export const constantRoutes = [
  /* {
    path: '/',
    component: Layout,
    redirect: '/dashboard',
    children: [{
      path: 'dashboard',
      name: 'Dashboard',
      component: Dashboard,
      meta: { title: 'Dashboard', icon: 'dashboard' }
    }]
  },*/

  {
    path: '/',
    // path: '/load',
    component: Layout,
    name: 'Load Pattern',
    meta: { title: 'Load Pattern', icon: 'example' },
    children: [
      {
        path: '',
        name: 'one_plate',
        component: One_plate,
        meta: { title: 'One Microplate' }
      },
      {
        path: 'two_plates',
        name: 'two_plates',
        component: Two_plates,
        meta: { title: 'Two Microplates' }
      }
    ]
  },

  {
    path: '/file_manager',
    component: Layout,
    children: [
      {
        path: 'index',
        name: 'file_manager',
        component: File_manager,
        meta: { title: 'File Manager', icon: 'drag' }
      }
    ]
  },
  {
    path: '/settings',
    component: Layout,
    children: [
      {
        path: 'index',
        name: 'settings',
        component: Settings,
        meta: { title: 'Settings', icon: 'gear' }
      }
    ]
  },
  {
    path: 'external-link',
    component: Layout,
    children: [
      {
        path: 'https://www.pertzlab.net/',
        meta: { title: 'Documentation', icon: 'education' }
      }
    ]
  }]

const createRouter = () => new Router({
  // mode: 'history', // require service support
  scrollBehavior: () => ({ y: 0 }),
  routes: constantRoutes
})

const router = createRouter()

// Detail see: https://github.com/vuejs/vue-router/issues/1234#issuecomment-357941465
export function resetRouter() {
  const newRouter = createRouter()
  router.matcher = newRouter.matcher // reset router
}

export default router
