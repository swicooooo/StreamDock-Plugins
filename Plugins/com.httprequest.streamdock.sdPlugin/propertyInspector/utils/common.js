class EventPlus {
  constructor() { this.event = new EventTarget(); }
  on(name, callback) { this.event.addEventListener(name, e => callback(e.detail)); }
  send(name, data) { this.event.dispatchEvent(new CustomEvent(name, { detail: data, bubbles: false, cancelable: false })); }
}

const $emit = new EventPlus();

const $ = (selector, isAll = false) => {
  if (isAll) return Array.from(document.querySelectorAll(selector));
  return document.querySelector(selector);
};

$.debounce = (fn, delay = 300) => {
  let timer = null;
  return function (...args) {
    clearTimeout(timer);
    timer = setTimeout(() => fn.apply(this, args), delay);
  };
};
