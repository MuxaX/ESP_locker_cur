import { createApp } from "vue";
import App from "./App.vue";
import { createRouter, createWebHistory } from "vue-router";
import AccessLog from "./components/AccessLog.vue";
import KeyManagement from "./components/KeyManagement.vue";

const router = createRouter({
  history: createWebHistory(),
  routes: [
    {
      path: "/",
      redirect: "/AccessLog", // Добавляем перенаправление с корня
    },
    {
      path: "/AccessLog",
      name: "AccessLog",
      component: AccessLog,
    },
    {
      path: "/KeyManagement",
      name: "KeyManagement",
      component: KeyManagement,
    },
    {
      path: "/:pathMatch(.*)*",
      redirect: "/AccessLog", // Перенаправляем все неизвестные пути
    },
  ],
});

const app = createApp(App);
app.use(router);
app.mount("#app");
