/**
 * Trading Journal - Equity Curve Chart Initialization (Owner: Member A)
 * Uses Chart.js to render an interactive cumulative equity curve.
 */

document.addEventListener('DOMContentLoaded', () => {
    const canvas = document.getElementById('equityChart');
    if (!canvas) {
        return;
    }

    fetch('/api/equity-curve')
        .then(response => {
            if (!response.ok) {
                throw new Error('Network response was not ok');
            }
            return response.json();
        })
        .then(data => {
            let labels = [];
            let values = [];
            let details = [];

            if (Array.isArray(data)) {
                if (data.length === 0) {
                    showEmptyChart(canvas);
                    return;
                }
                data.forEach((item, index) => {
                    if (typeof item === 'object') {
                        const dateLabel = item.date ? item.date : `Trade #${index + 1}`;
                        const tickerLabel = item.ticker ? ` (${item.ticker})` : '';
                        labels.push(dateLabel + tickerLabel);
                        values.push(item.equity !== undefined ? item.equity : 0);
                        details.push(item);
                    } else {
                        labels.push(`Trade #${index + 1}`);
                        values.push(item);
                        details.push({ equity: item });
                    }
                });
            } else if (data && data.equity && Array.isArray(data.equity)) {
                values = data.equity;
                labels = data.labels || values.map((_, i) => `Trade #${i + 1}`);
                details = data.trades || [];
            }

            if (values.length === 0) {
                showEmptyChart(canvas);
                return;
            }

            if (typeof Chart === 'undefined') {
                showEmptyChart(canvas, 'Chart library unavailable');
                return;
            }

            const ctx = canvas.getContext('2d');
            const finalVal = values[values.length - 1];
            const isProfitable = finalVal >= 0;
            const primaryColor = isProfitable ? '#198754' : '#dc3545';

            // Create smooth vertical gradient under line
            const gradientFill = ctx.createLinearGradient(0, 0, 0, 350);
            gradientFill.addColorStop(0, isProfitable ? 'rgba(25, 135, 84, 0.28)' : 'rgba(220, 53, 69, 0.28)');
            gradientFill.addColorStop(1, 'rgba(255, 255, 255, 0.0)');

            if (canvas._chartInstance) {
                canvas._chartInstance.destroy();
            }

            canvas._chartInstance = new Chart(ctx, {
                type: 'line',
                data: {
                    labels: labels,
                    datasets: [{
                        label: 'Cumulative Net Equity (₹)',
                        data: values,
                        borderColor: primaryColor,
                        backgroundColor: gradientFill,
                        fill: true,
                        tension: 0.25,
                        pointBackgroundColor: primaryColor,
                        pointBorderColor: '#ffffff',
                        pointBorderWidth: 2,
                        pointRadius: 4,
                        pointHoverRadius: 7,
                        borderWidth: 2.5
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    interaction: {
                        mode: 'index',
                        intersect: false
                    },
                    plugins: {
                        legend: {
                            display: false
                        },
                        tooltip: {
                            backgroundColor: 'rgba(33, 37, 41, 0.95)',
                            padding: 12,
                            titleFont: { size: 13, weight: 'bold' },
                            bodyFont: { size: 12 },
                            callbacks: {
                                label: function (context) {
                                    const val = context.parsed.y;
                                    const sign = val >= 0 ? '+' : '';
                                    return `Equity: ${sign}₹${val.toLocaleString(undefined, {
                                        minimumFractionDigits: 2,
                                        maximumFractionDigits: 2
                                    })}`;
                                },
                                afterLabel: function (context) {
                                    const idx = context.dataIndex;
                                    if (details[idx] && details[idx].profit !== undefined) {
                                        const p = details[idx].profit;
                                        const sign = p >= 0 ? '+' : '';
                                        const ticker = details[idx].ticker ? ` (${details[idx].direction || ''} ${details[idx].ticker})` : '';
                                        return `Trade P&L: ${sign}₹${p.toFixed(2)}${ticker}`;
                                    }
                                    return '';
                                }
                            }
                        }
                    },
                    scales: {
                        x: {
                            grid: {
                                color: 'rgba(0, 0, 0, 0.04)'
                            },
                            ticks: {
                                maxRotation: 45,
                                autoSkip: true,
                                maxTicksLimit: 12,
                                font: { size: 11 }
                            }
                        },
                        y: {
                            grid: {
                                color: 'rgba(0, 0, 0, 0.06)'
                            },
                            ticks: {
                                font: { size: 11 },
                                callback: function (value) {
                                    return (value >= 0 ? '₹' : '-₹') + Math.abs(value).toLocaleString();
                                }
                            }
                        }
                    }
                }
            });
        })
        .catch(err => {
            console.error('Error fetching equity curve:', err);
            showEmptyChart(canvas, 'Failed to load chart data');
        });

    function showEmptyChart(canvasElement, message = 'No trades recorded yet') {
        const parent = canvasElement.parentElement;
        if (parent) {
            parent.innerHTML = `
                <div class="d-flex flex-column align-items-center justify-content-center h-100 text-muted py-5">
                    <i class="bi bi-graph-up display-5 mb-2 opacity-50"></i>
                    <p class="mb-2 fw-medium">${message}</p>
                    <a href="/trades/new" class="btn btn-sm btn-primary">
                        <i class="bi bi-plus-circle me-1"></i>Log Your First Trade
                    </a>
                </div>
            `;
        }
    }
});
