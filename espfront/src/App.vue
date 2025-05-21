<template>
  <div id="app">
    <!--<div class="debug-info">
      <p>Текущий маршрут: {{ $route.name || "не определен" }}</p>
      <router-link to="/AccessLog">Журнал доступа</router-link> |
      <router-link to="/KeyManagement">Управление ключами</router-link>
    </div>-->
    <router-view v-if="isRouterReady" />
    <div v-else class="loading-router">Загрузка маршрутизатора...</div>
  </div>
</template>

<script>
export default {
  name: "App",
  data() {
    return {
      isRouterReady: false,
    };
  },
  mounted() {
    this.isRouterReady = true;
    console.log("Текущий маршрут:", this.$route);
  },
};
</script>

<style>
#app {
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  color: #2c3e50;
  padding: 20px;
}

.debug-info {
  background: #f5f5f5;
  padding: 10px;
  margin-bottom: 20px;
  border-radius: 4px;
}

.loading-router {
  padding: 20px;
  text-align: center;
}

a {
  margin: 0 10px;
  color: #42b983;
}
</style>
