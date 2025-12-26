const app = {
    data: null,
    currentRaceIndex: 0,

    init: function () {
        if (!window.raceData) {
            alert("No data found! Run build.bat to generate data.js");
            return;
        }

        this.data = window.raceData;

        // Find last race index as default
        if (this.data.races.length > 0) {
            this.currentRaceIndex = 0; // Start at Race 1
        }

        this.cacheDOM();
        this.bindEvents();
        this.renderDashboard();
    },

    cacheDOM: function () {
        this.dom = {
            raceName: document.getElementById('race-name'),
            raceTimer: document.getElementById('session-timer'),
            raceTable: document.getElementById('race-table-body'),
            raceWeather: document.getElementById('race-weather'),
            raceTemp: document.getElementById('race-temp'),
            prevBtn: document.getElementById('prev-race'),
            nextBtn: document.getElementById('next-race'),
            currentRaceDisplay: document.getElementById('current-race-display'),
            standingsTable: document.getElementById('standings-table-body'),
            navItems: document.querySelectorAll('.nav-item'),
            views: {
                dashboard: document.getElementById('view-dashboard'),
                standings: document.getElementById('view-standings')
            }
        };
    },

    bindEvents: function () {
        // Navigation
        this.dom.navItems.forEach(item => {
            item.addEventListener('click', (e) => {
                e.preventDefault();
                const view = e.target.dataset.view;
                this.switchView(view);

                // Update active class
                this.dom.navItems.forEach(n => n.classList.remove('active'));
                e.target.classList.add('active');
            });
        });

        // Race Controls
        this.dom.prevBtn.addEventListener('click', () => {
            if (this.currentRaceIndex > 0) {
                this.currentRaceIndex--;
                this.renderDashboard();
            }
        });

        this.dom.nextBtn.addEventListener('click', () => {
            if (this.currentRaceIndex < this.data.races.length - 1) {
                this.currentRaceIndex++;
                this.renderDashboard();
            }
        });
    },

    switchView: function (viewName) {
        Object.values(this.dom.views).forEach(el => el.style.display = 'none');
        this.dom.views[viewName].style.display = 'block';

        if (viewName === 'standings') {
            this.renderStandings();
            this.dom.raceName.textContent = "Season Standings";
            this.dom.raceTimer.textContent = "COMPLETE";
        } else {
            this.renderDashboard();
        }
    },

    getTeamColor: function (team) {
        const colors = {
            "Red Bull": "#3671C6",
            "Mercedes": "#27F4D2",
            "Ferrari": "#E80020",
            "McLaren": "#FF8000",
            "Aston Martin": "#22594D",
            "Alpine": "#0093CC",
            "Williams": "#64C4FF",
            "Haas": "#B6BABD",
            "Kick Sauber": "#52E252",
            "RB": "#6692FF"
        };
        return colors[team] || "#FFF";
    },

    formatTime: function (seconds) {
        const mins = Math.floor(seconds / 60);
        const secs = (seconds % 60).toFixed(3);
        return `${mins}:${secs.padStart(6, '0')}`;
    },

    getTempForWeather: function (weather) {
        if (!weather) return "25°C";
        if (weather === "Rain") return "18°C";
        if (weather === "Cloudy") return "22°C";
        if (weather === "Sunny") return "32°C";
        return "26°C";
    },

    renderDashboard: function () {
        const race = this.data.races[this.currentRaceIndex];
        if (!race) return;

        // Header
        this.dom.raceName.textContent = `${race.trackName} Grand Prix`;
        this.dom.currentRaceDisplay.textContent = `Round ${race.raceId}`;

        // Weather
        this.dom.raceWeather.textContent = race.weather || "Sunny";

        // Table
        this.dom.raceTable.innerHTML = '';

        const leaderTime = race.results[0].time;

        race.results.forEach((res, index) => {
            const tr = document.createElement('tr');
            tr.className = `pos-${res.position}`;

            // DNF Detection (Gap > 400s)
            let isDNF = false;
            if (index > 0 && (res.time - leaderTime > 400)) {
                isDNF = true;
                tr.classList.add('dnf-row'); // You'll need to add this class to CSS
            }

            // Gap Calculation
            let gapStr = "";
            if (isDNF) {
                gapStr = "DNF";
            } else if (index === 0) {
                gapStr = this.formatTime(res.time);
            } else {
                const diff = res.time - leaderTime;
                gapStr = `+${diff.toFixed(3)}`;
            }

            const teamColor = this.getTeamColor(res.team);

            tr.innerHTML = `
                <td>
                    <div class="pos-badge">${res.position}</div>
                </td>
                <td style="border-left: 4px solid ${teamColor}; padding-left: 10px;">
                    ${res.name}
                    <span style="color: #666; font-size: 12px; margin-left: 5px;">${res.team.toUpperCase()}</span>
                </td>
                <td style="color: ${index === 0 ? '#fff' : (isDNF ? '#ff4d4d' : '#c5c6c7')}">${gapStr}</td>
                <td style="color: ${isDNF ? '#666' : '#fff'}">${this.formatTime(res.time)}</td>
                <td><span style="color: var(--accent)">${res.pits}</span></td>
                <td>
                    <span style="color: ${res.tyreDegradation < 40 ? '#4caf50' : res.tyreDegradation < 70 ? '#ffeb3b' : '#f44336'}">
                        ${res.tyreDegradation}%
                    </span>
                </td>
                <td style="color: #fff; font-weight: bold;">${res.overtakes !== undefined ? res.overtakes : 0}</td>
                <td>${res.points > 0 ? `+${res.points}` : ''}</td>
            `;
            this.dom.raceTable.appendChild(tr);
        });
    },

    renderStandings: function () {
        this.dom.standingsTable.innerHTML = '';

        this.data.standings.forEach(driver => {
            const tr = document.createElement('tr');
            const teamColor = this.getTeamColorVar(driver.team);

            tr.innerHTML = `
                <td><div class="pos-badge" style="background: transparent; border: 1px solid #333">${driver.rank}</div></td>
                <td>
                     <span class="team-bar" style="background-color: var(${teamColor})"></span>
                     ${driver.name}
                </td>
                <td>${driver.team}</td>
                <td style="color: var(--accent); font-weight: bold;">${driver.points}</td>
                <td>${this.formatTime(driver.totalTime)}</td>
            `;
            this.dom.standingsTable.appendChild(tr);
        });
    }
};

window.addEventListener('DOMContentLoaded', () => {
    app.init();
});
