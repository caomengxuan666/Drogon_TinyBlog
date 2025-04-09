new Vue({
    el: "#app",
    data() {
        return {
            audio: null,
            circleLeft: null,
            barWidth: null,
            duration: null,
            currentTime: null,
            isTimerPlaying: false,
            restorePending: false, // 新增状态恢复标记
            tracks: [
                {
                    name: "你若三冬",
                    artist: "一博",
                    cover: "/static/music/nrsd.png",
                    source: "/static/music/nrsd.mp3",
                    url: "",
                    favorited: false
                },
                {
                    name: "天真的橡皮",
                    artist: "白水寒", 
                    cover: "/static/music/tzdxp.png",
                    source: "/static/music/tzdxp.mp3",
                    url: "",
                    favorited: true
                },
                {
                    name: "Extreme Ways",
                    artist: "Moby",
                    cover: "https://raw.githubusercontent.com/muhammederdem/mini-player/master/img/3.jpg",
                    source: "https://raw.githubusercontent.com/muhammederdem/mini-player/master/mp3/3.mp3",
                    url: "https://www.youtube.com/watch?v=ICjyAe9S54c",
                    favorited: false
                },
                {
                    name: "Butterflies",
                    artist: "Sia",
                    cover: "https://raw.githubusercontent.com/muhammederdem/mini-player/master/img/4.jpg",
                    source: "https://raw.githubusercontent.com/muhammederdem/mini-player/master/mp3/4.mp3",
                    url: "https://www.youtube.com/watch?v=kYgGwWYOd9Y",
                    favorited: false
                },
                {
                    name: "The Final Victory",
                    artist: "Haggard",
                    cover: "https://raw.githubusercontent.com/muhammederdem/mini-player/master/img/5.jpg",
                    source: "https://raw.githubusercontent.com/muhammederdem/mini-player/master/mp3/5.mp3",
                    url: "https://www.youtube.com/watch?v=0WlpALnQdN8",
                    favorited: true
                },
                {
                    name: "Genius ft. Sia, Diplo, Labrinth",
                    artist: "LSD",
                    cover: "https://raw.githubusercontent.com/muhammederdem/mini-player/master/img/6.jpg",
                    source: "https://raw.githubusercontent.com/muhammederdem/mini-player/master/mp3/6.mp3",
                    url: "https://www.youtube.com/watch?v=HhoATZ1Imtw",
                    favorited: false
                },
                {
                    name: "The Comeback Kid",
                    artist: "Lindi Ortega",
                    cover: "https://raw.githubusercontent.com/muhammederdem/mini-player/master/img/7.jpg",
                    source: "https://raw.githubusercontent.com/muhammederdem/mini-player/master/mp3/7.mp3",
                    url: "https://www.youtube.com/watch?v=me6aoX0wCV8",
                    favorited: true
                },
                {
                    name: "Overdose",
                    artist: "Grandson",
                    cover: "https://raw.githubusercontent.com/muhammederdem/mini-player/master/img/8.jpg",
                    source: "https://raw.githubusercontent.com/muhammederdem/mini-player/master/mp3/8.mp3",
                    url: "https://www.youtube.com/watch?v=00-Rl3Jlx-o",
                    favorited: false
                },
                {
                    name: "Rag'n'Bone Man",
                    artist: "Human",
                    cover: "https://raw.githubusercontent.com/muhammederdem/mini-player/master/img/9.jpg",
                    source: "https://raw.githubusercontent.com/muhammederdem/mini-player/master/mp3/9.mp3",
                    url: "https://www.youtube.com/watch?v=L3wKzyIN1yk",
                    favorited: false
                }
            ],
            currentTrack: null,
            currentTrackIndex: 0,
            transitionName: null
        };
    },
    methods: {
        play() {
            if (this.audio.paused) {
                this.audio.play();
                this.isTimerPlaying = true;
            } else {
                this.audio.pause();
                this.isTimerPlaying = false;
            }
        },
        generateTime() {
            let width = (100 / this.audio.duration) * this.audio.currentTime;
            this.barWidth = width + "%";
            this.circleLeft = width + "%";
            let durmin = Math.floor(this.audio.duration / 60);
            let dursec = Math.floor(this.audio.duration - durmin * 60);
            let curmin = Math.floor(this.audio.currentTime / 60);
            let cursec = Math.floor(this.audio.currentTime - curmin * 60);
            if (durmin < 10) {
                durmin = "0" + durmin;
            }
            if (dursec < 10) {
                dursec = "0" + dursec;
            }
            if (curmin < 10) {
                curmin = "0" + curmin;
            }
            if (cursec < 10) {
                cursec = "0" + cursec;
            }
            this.duration = durmin + ":" + dursec;
            this.currentTime = curmin + ":" + cursec;
        },
        updateBar(x) {
            let progress = this.$refs.progress;
            let maxduration = this.audio.duration;
            let position = x - progress.offsetLeft;
            let percentage = (100 * position) / progress.offsetWidth;
            if (percentage > 100) {
                percentage = 100;
            }
            if (percentage < 0) {
                percentage = 0;
            }
            this.barWidth = percentage + "%";
            this.circleLeft = percentage + "%";
            this.audio.currentTime = (maxduration * percentage) / 100;
            this.audio.play();
        },
        clickProgress(e) {
            this.isTimerPlaying = true;
            this.audio.pause();
            this.updateBar(e.pageX);
        },
        prevTrack() {
            this.transitionName = "scale-in";
            this.isShowCover = false;
            if (this.currentTrackIndex > 0) {
                this.currentTrackIndex--;
            } else {
                this.currentTrackIndex = this.tracks.length - 1;
            }
            this.currentTrack = this.tracks[this.currentTrackIndex];
            this.resetPlayer();
        },
        nextTrack() {
            this.transitionName = "scale-out";
            this.isShowCover = false;
            if (this.currentTrackIndex < this.tracks.length - 1) {
                this.currentTrackIndex++;
            } else {
                this.currentTrackIndex = 0;
            }
            this.currentTrack = this.tracks[this.currentTrackIndex];
            this.resetPlayer();
        },
        resetPlayer() {
            this.barWidth = 0;
            this.circleLeft = 0;
            this.audio.currentTime = 0;
            this.audio.src = this.currentTrack.source;
            setTimeout(() => {
                if (this.isTimerPlaying) {
                    this.audio.play();
                } else {
                    this.audio.pause();
                }
            }, 300);
        },
        favorite() {
            this.tracks[this.currentTrackIndex].favorited = !this.tracks[
                this.currentTrackIndex
            ].favorited;
        },
        broadcastState() {
            const state = {
                currentTime: this.audio.currentTime,
                playing: !this.audio.paused,
                volume: this.audio.volume,
                index: this.currentTrackIndex,
                duration: this.audio.duration
            };

            // 通过postMessage通知父窗口
            window.parent.postMessage({
                type: 'playerState',
                payload: state
            }, '*');

            // 保存到sessionStorage
            sessionStorage.setItem('playerState', JSON.stringify(state));
        },
        // 新增状态恢复方法
        restoreState(state) {
            if (this.audio.readyState < 2) { // 等待元数据加载
                this.restorePending = true;
                this.audio.addEventListener('loadedmetadata', () => {
                    this.applyState(state);
                });
            } else {
                this.applyState(state);
            }
        },
        applyState(state) {
            this.audio.currentTime = state.currentTime;
            this.audio.volume = state.volume;
            this.currentTrackIndex = state.index;
            this.currentTrack = this.tracks[state.index];

            if (state.playing) {
                this.audio.play().catch(() => { }); // 自动处理播放错误
            }
        },
        watch: {
            isTimerPlaying(newVal) {
                this.broadcastState();
            },
            currentTrackIndex(newVal) {
                this.broadcastState();
            }
        }

    },
    created() {
        let vm = this;
        this.currentTrack = this.tracks[0];
        this.audio = new Audio();
        this.audio.src = this.currentTrack.source;
        this.audio.ontimeupdate = function () {
            vm.generateTime();
        };
        this.audio.onloadedmetadata = function () {
            vm.generateTime();
        };
        this.audio.onended = function () {
            vm.nextTrack();
            this.isTimerPlaying = true;
        };

        // this is optional (for preload covers)
        for (let index = 0; index < this.tracks.length; index++) {
            const element = this.tracks[index];
            let link = document.createElement('link');
            link.rel = "prefetch";
            link.href = element.cover;
            link.as = "image"
            document.head.appendChild(link)
        }
    },
    mounted() {
        // 监听来自父窗口的状态恢复请求
        window.addEventListener('message', (e) => {
            if (e.data.type === 'restoreState') {
                const state = e.data.payload;
                this.restoreState(state);
            }
        });

        // 监听音频元数据加载完成
        this.audio.addEventListener('loadedmetadata', () => {
            if (this.restorePending) {
                this.restorePending = false;
                const state = JSON.parse(sessionStorage.getItem('playerState'));
                this.applyState(state);
            }
        });
    }
});